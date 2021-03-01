#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <conio.h>
#include <dos.h>

#include "config.h"
#include "debug.h"
#include "idatypes.h"
#include "lottypes.h"
#include "lotcalls.h"
#include "bundle.h"
#include "lmbcs.h"
#include "attr.h"

static int curx;
static int cury;
static unsigned char far * const framebuffer = MK_FP(0xb800, 0x0000);

static unsigned char blanks[MAX_COLS];

static unsigned char far *attrmap;

static struct DISPLAYINFO dpinfo = {
    246,    // num_text_cols
    30,     // num_text_rows
    true,   // graphics
    true,   // full_screen_graph
    1,      // hpu_per_col
    320,    // graph_cols
    200,    // graph_rows
    6,      // graph_col_res
    5,      // graph_row_res
    0,      // view_set_size
    true,   // iscolor
};

static struct DEVDATA devdata = {
  1,        // ShowMeFlag
  200,      // RasterHeight
  6,        // AspectX
  5,        // AspectY
  4,        // RHmusPerTHmu
  1,        // RHmuPerGHmus
  8,        // RVmusPerTVmu
  1,        // RVmuPerGVmus
  {
      { { 1,  8  }, { 0, 0 }, 0, { 0, 0 } },  // 0
      { { 16, 8  }, { 0, 0 }, 0, { 0, 0 } },  // 1
      { { 32, 16 }, { 0, 0 }, 0, { 0, 0 } },  // 2
      { { 32, 16 }, { 0, 0 }, 0, { 0, 0 } },  // 3
      { { 32, 16 }, { 0, 0 }, 0, { 0, 0 } },  // 4
      { { 16, 8  }, { 0, 0 }, 0, { 0, 0 } },  // 5
      { { 16, 8  }, { 0, 0 }, 0, { 0, 0 } },  // 6
      { { 16, 8  }, { 0, 0 }, 0, { 0, 0 } },  // 7
      { { 16, 8  }, { 0, 0 }, 0, { 0, 0 } },  // 8
      { { 16, 8  }, { 0, 0 }, 0, { 0, 0 } },  // 9
      { { 16, 8  }, { 0, 0 }, 0, { 0, 0 } },  // 10
      { { 16, 8  }, { 0, 0 }, 0, { 0, 0 } },  // 11
  },        // FillPatts
  {
    0xAA, 0xAA, 0x55, 0x00, 0xAA, 0x55, 0x00, 0x00,
    0xFF, 0xAA, 0x55, 0x00, 0xAA, 0x55, 0x00, 0xFF,
  },        // ColorMap
  {
    0xFFFF, 0xAAAA, 0xCCCC, 0xCACA,
    0xC0C0, 0x00FF, 0xFF24, 0x0000
  },        // SrcPatD
  {
    0x0001, 0x0006, 0x000C, 0x0012,
    0x0018, 0x001E, 0x0024, 0x002A
  },        // SrcPatS
  8,        // FIndexCnt
  NULL,     // FontIndex
  6,        // FNamesCnt
  0,        // FontNames
  {
    0,      // scan_linx
    0,      // fill_rect
    0,      // thin_diag_line
    0,      // thin_vert_line
    0,      // shade_rect
    0,      // fill_scan_list
  },        // DevFuncs
};

struct LOTUSFUNCS far *callbacks;

void __pascal GetDisplayInfo(struct DISPLAYINFO far *dspinfo)
{
    traceent("GetDisplayInfo");
    traceptr(dspinfo);

    //callbacks->MapMemVmr(&dpinfo, 1);
    memcpy(dspinfo, &dpinfo, sizeof dpinfo);

    tracebuf(dspinfo, sizeof dpinfo);

    return;
}

int RegisterDevdata()
{
    int result = 0;
    int retcode;

    trace("registering DEVDATA");

    tracebuf(&devdata, sizeof devdata);
    traceint(devdata.ShowMeFlag);

    retcode = callbacks->RegisterDevdataStruct(&devdata, &result);

    traceint(retcode);
    traceint(result);
    return 0;
}

int __pascal DriverInit(void *drvapi, void *vbdptr, char *cfgname, char deflmbcsgrp)
{
    openlog("H:\\DEBUG.LOG");
    traceent("DriverInit");
    traceptr(drvapi);
    traceptr(vbdptr);
    tracestr(cfgname);
    tracechr(deflmbcsgrp);

    // Save this pointer for calling lotus functions.
    callbacks = drvapi;

    tracebuf(callbacks, sizeof *callbacks);

    // Create a line of blanks for quickly clearing a row.
    memset(blanks, ' ', sizeof blanks);

    // Setup the CGA <=> 123 attribute map.
    attrmap = callbacks->Allocate(dpinfo.num_text_cols * dpinfo.num_text_rows, 0);

    traceptr(attrmap);

    // Out of memory?
    if (attrmap == NULL) {
        logmsg("failed to allocate space for the attribute map");
        return 1;
    }

    // Initialize to white on black.
    memset(attrmap, 0, dpinfo.num_text_cols * dpinfo.num_text_rows);

    // Parse out configuration bundle.
    ParseConfig(vbdptr);

    RegisterDevdata();
    return 0;
}

int __pascal DriverTerminate()
{
    traceent("DriverTerminate");

    callbacks->Free(attrmap, 0, dpinfo.num_text_cols * dpinfo.num_text_rows);
    closelog();

    return 0;
}

int __pascal ResetDisplay()
{
    traceent("ResetDisplay");

    return 1;
}

int __pascal SetGraphicsMode()
{
    traceent("SetGraphicsMode");
    return 1;
}

void __pascal MoveCursor(int col, int line)
{
    traceent("MoveCursor");
    traceint(col);
    traceint(line);

    if (col < 0) {
        col += dpinfo.num_text_cols;
    }

    curx = col;
    cury = line;

    return;
}

// Mask is the bits that I'm allowed to change.
int WriteStringToFramebuffer(unsigned char far *str,
                             int len,
                             unsigned char attrs,
                             unsigned char mask)
{
    unsigned char far *line;
    unsigned char far *attr;
    int i;

    trace("WriteStringToFramebuffer");
    tracestrlen(str, len);
    traceint(len);
    traceptr(str);
    traceint(curx);
    traceint(cury);

    // It's easier to parse lotus attributes in octal.
    traceval("%03ho", mask);
    traceval("%03ho", attrs);

    // That doesn't work for CGA attributes, hex is better.
    traceval("%#hx", LATTR(attrs));

    for (i = 0; i < len; i++) {
        // Current line in framebuffer.
        line = (framebuffer + cury * dpinfo.num_text_cols * 2);
        attr = (attrmap + cury * dpinfo.num_text_cols * 1);

        // Optionally write character, it's possible the caller
        // only wanted to change attributes.
        if (str) {
            line[curx * 2 + 0] = str[i];
        }

        // If specified, change the character attributes.
        //
        // - Sometimes 123 wants to change the FG/BG attributes, but leave the
        //   other unchanged, so a mask can be specified.
        //   ATTR_ALL will change both
        //   ATTR_NONE will change none
        //   ATTR_FG will change FG, leaving BG
        //   ATTR_BG will change BG, leaving FG
        //
        // - The parameter is a *LOTUS* attribute, we need to translate it
        //   to a CGA attribute before writing it to the framebuffer.
        //
        // - This gets tricky, because what if 123 wants to change the FG only?
        //
        //      1. Read the current attribute.
        //      2. Translate it into a lotus attribute.
        //      3. Apply masks.
        //      4. Or in any new bits.
        //      5. Translate it back into a CGA attr
        //      6. Write it into the framebuffer.
        if (mask == ATTR_ALL) {
            // The easy case, overwrite everything.
            line[curx * 2 + 1] = LATTR(attr[curx] = attrs);
        } else if (mask != ATTR_NONE) {
            unsigned char curattr = attr[curx];

            // Now remove the bits we're changing.
            curattr &= ~mask;

            // Now remove any bits from new attributes we dont want.
            attrs &= mask;

            // Combine them.
            attrs |= curattr;

            // Translate and write them back.
            line[curx * 2 + 1] = LATTR(attr[curx] = attrs);
        }

        // Advance cursor position.
        curx++;

        curx %= dpinfo.num_text_cols;
        cury += curx == 0;
    }

    return curx;
}

int __pascal WriteLmbcsStringWithAttributes(int byteslen,
                                            unsigned char far *lmbcsptr,
                                            unsigned char attrs)
{
    unsigned char dest[MAX_COLS];
    int result;

    traceent("WriteLmbcsStringWithAttributes");
    traceint(byteslen);
    tracestrlen(lmbcsptr, byteslen);
    traceval("%03ho", attrs);

    result = translate_lmbcs(lmbcsptr, dest, sizeof dest, byteslen, 1);

    WriteStringToFramebuffer(dest, result, MKFG(attrs), ATTR_FG);

    traceint(result);

    return result;
}

int __pascal WritePaddedLmbcsStringWithAttributes(int byteslen,
                                                  char far *lmbcsptr,
                                                  unsigned char attrs,
                                                  int startpad,
                                                  int endpad)
{
    unsigned char dest[MAX_COLS];
    int result;
    traceent("WritePaddedLmbcsStringWithAttributes");
    traceint(byteslen);
    tracestrlen(lmbcsptr, byteslen);
    traceval("%03ho", attrs);
    traceint(startpad);
    traceint(endpad);

    if (endpad < 0) {
        endpad += dpinfo.num_text_cols;
    }

    memset(dest, ' ', sizeof dest);

    result = translate_lmbcs(lmbcsptr, dest + startpad, sizeof(dest) - startpad, byteslen, 1);

    traceint(result);

    // Write the string
    WriteStringToFramebuffer(dest, endpad, MKFG(attrs), ATTR_FG);

    return endpad;
}

// The region is a rectangle:
//
//      curx,cury           curx+cols,cury
//          +------------------------+
//          |                        |
//          |                        |
//          |                        |
//          +------------------------+
// curx,cury+lines          curx+cols,cury+lines
//
// Does not appear to change current cursor position.

void __pascal SetRegionBgAttributes(int cols, int lines, char attrs)
{
    int y;
    int origx;
    int origy;
    traceent("SetRegionBgAttributes");

    traceint(cols);
    traceint(lines);

    traceint(curx);
    traceint(cury);

    traceval("%03ho", attrs);

    origx = curx;
    origy = cury;

    for (y = 0; y < lines; y++) {
        WriteStringToFramebuffer(NULL, cols, MKBG(attrs), ATTR_BG);
        curx = origx;
        cury = origy + y;
    }

    curx = origx;
    cury = origy;

    return;
}

void __pascal ClearRegionForeground(int cols,
                                    int lines,
                                    unsigned char attrs)
{
    int y;
    int origx;
    int origy;
    traceent("ClearRegionForeground");

    traceint(cols);
    traceint(lines);

    traceint(curx);
    traceint(cury);

    traceval("%03ho", attrs);

    origx = curx;
    origy = cury;
    for (y = 0; y < lines; y++) {
        WriteStringToFramebuffer(blanks, cols, MKBG(attrs), ATTR_ALL);
        curx = origx;
        cury = origy + y;
    }

    curx = origx;
    cury = origy;

    return;
}

void __pascal ClearRegionForegroundKeepBg(int cols, int lines)
{
    int y;
    int origx;
    int origy;

    traceent("ClearRegionForegroundKeepBg");

    traceint(cols);
    traceint(lines);

    traceint(curx);
    traceint(cury);

    origx = curx;
    origy = cury;
    for (y = 0; y < lines; y++) {
        WriteStringToFramebuffer(blanks, cols, 0000, ATTR_FG);
        curx = origx;
        cury = origy + y;
    }

    curx = origx;
    cury = origy;

    return;
}

void __pascal BlockRegionCopy(int topx, int topy, int botx, int boty)
{
    traceent("BlockRegionCopy");
    traceint(topx);
    traceint(topy);
    traceint(botx);
    traceint(boty);
    __debugbreak();
}

int __pascal CalcSizeTranslatedString(int argstrlen, char far *argstr)
{
    int result;
    traceent("CalcSizeTranslatedString");
    tracestrlen(argstr, argstrlen);
    traceptr(argstr);

    result = translate_lmbcs(argstr, NULL, 0, argstrlen, 1);

    traceint(result);
    return result;
}

int __pascal FitTranslatedString(int strarglen,
                                 char far *strarg,
                                 int ncols,
                                 int far *bytesneeded)
{
    int result;

    traceent("FitTranslatedString");
    traceint(strarglen);
    tracestrlen(strarg, strarglen);
    traceint(ncols);
    traceptr(bytesneeded);

    if (ncols < 0) {
        ncols += dpinfo.num_text_cols;
    }

    result = translate_lmbcs(strarg, NULL, 0, strarglen, 1);

    traceint(result);

    *bytesneeded = result;

    traceint(*bytesneeded);

    return result;
}

void __pascal SetCursorInvisible()
{
    traceent("SetCursorInvisible");
    return;
}

void __pascal SetCursorVisible()
{
    traceent("SetCursorVisible");
    return;
}

void __pascal LockCursorAttributes()
{
    traceent("LockCursorAttributes");
    return;
}

void __pascal QQCalledBeforeGraphicsMode()
{
    traceent("QQCalledBeforeGraphicsMode");
    __debugbreak();
}

void __pascal QQLotusApiJustCallsFunc_1()
{
    traceent("QQLotusApiJustCallsFunc_1");
    __debugbreak();
}

void __pascal MoveCursor2(int col, int line)
{
    traceent("MoveCursor2");
    MoveCursor(col, line);
    return;
}

void __pascal QQLotusApiJustCallsFunc_0()
{
    traceent("QQLotusApiJustCallsFunc_0");
    __debugbreak();
    return;
}

int __pascal ComplicatedNop()
{
    traceent("ComplicatedNop");
    __debugbreak();
    return 0;
}

int __pascal zerosub_0()
{
    traceent("zerosub_0");
    __debugbreak();
    return 0;
}

int __pascal LotusApiPassedPtr()
{
    traceent("LotusApiPassedPtr");
    __debugbreak();
    return 0;
}

int __pascal LotusApiJustCallsFunc()
{
    traceent("LotusApiJustCallsFunc");
    __debugbreak();
    return 0;
}

int __pascal nullsub_5()
{
    traceent("nullsub_5");
    __debugbreak();
    return 0;
}
