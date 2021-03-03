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

// The current cursor position.
static int curx;
static int cury;

// LDT entries we need to release on exit.
unsigned fbseg = CGA_FRAMEBUF;
unsigned vbseg = BIOS_DATA_AREA;

// This will point into the CGA framebuffer.
static unsigned char far * framebuffer;

// These two pointers reference the BDA (BIOS Data Area)
// The number of rows will be 1 less than the actual rows. Look, I didn't
// invent it.
static unsigned int far * bdcols;
static unsigned char far * bdrows;


// There is an "optimized" LMBCS encoding that skips the group byte
// and just uses a default, and 123 tells us what the default should
// be at startup.
static unsigned char lmbcsgroup;

// A handle returned when registering DEVDATA.
static unsigned devdatahdl;

static unsigned char blanks[MAX_COLS];

// This will point at fixed (not vmr) memory. vmrs are 123's
// abstraction of XMS/EMS handles, they need to be mapped before
// you can touch them.
static unsigned char far *attrmap;

static struct DISPLAYINFO dpinfo = {
    80,     // num_text_cols
    25,     // num_text_rows
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

// I think an mu must be a "movement unit"?
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

    memcpy(dspinfo, &dpinfo, sizeof dpinfo);

    tracebuf(dspinfo, sizeof dpinfo);

    return;
}

int RegisterDevdata()
{
    int result;
    void *destptr;

    trace("registering DEVDATA");

    tracebuf(&devdata, sizeof devdata);

    // Allocate a VMR buffer to give to 123.
    destptr = callbacks->AllocMem(0x27, sizeof devdata, 1);

    // Copy out buffer over.
    memcpy(destptr, &devdata, sizeof devdata);

    // Register it, we release this handle in our DriverTerminate().
    devdatahdl = callbacks->RegisterDevdataStruct(destptr, &result);

    traceint(devdatahdl);
    traceint(result);
    return 0;
}

int __pascal DriverInit(void *drvapi, void *vbdptr, char *cfgname, char deflmbcsgrp)
{
    // Only log debugging messages for debug builds.
#ifndef RELEASE
    openlog("H:\\DEBUG.LOG");
#endif
    traceent("DriverInit");
    traceptr(drvapi);
    traceptr(vbdptr);
    tracestr(cfgname);
    tracechr(deflmbcsgrp);

    // Save this pointer for calling lotus functions.
    callbacks = drvapi;

    tracebuf(callbacks, sizeof *callbacks);

    // Record the default lmbcs group
    lmbcsgroup = deflmbcsgrp;

    // Create a line of blanks for quickly clearing a row.
    memset(blanks, ' ', sizeof blanks);

    if (!bdrows && !bdcols) {
        // Request Bios Data Area access.
        callbacks->GetDescriptor(&vbseg, 0, 0x100);

        traceint(vbseg);

        // See https://stanislavs.org/helppc/bios_data_area.html
        bdrows = MK_FP(vbseg, 0x84);
        bdcols = MK_FP(vbseg, 0x4A);

        traceint(*bdrows);
        traceint(*bdcols);

        dpinfo.num_text_cols = *bdcols;
        dpinfo.num_text_rows = *bdrows + 1;
    }

    traceint(dpinfo.num_text_cols);
    traceint(dpinfo.num_text_rows);

    if (!framebuffer) {
        // Request an LDT for framebuffer access.
        callbacks->GetDescriptor(&fbseg, 0, dpinfo.num_text_cols * dpinfo.num_text_rows * 2);

        traceint(fbseg);

        framebuffer = MK_FP(fbseg, 0);
    }

    // Dump a few bytes just to make sure this look sane.
    tracebuf(framebuffer, 32);

    if (attrmap == NULL) {
        // Setup the CGA <=> 123 attribute map.
        attrmap = callbacks->Allocate(dpinfo.num_text_cols * dpinfo.num_text_rows, 0);

        // Out of memory?
        if (attrmap == NULL) {
            logmsg("failed to allocate space for the attribute map");
            return 1;
        }
    }

    traceptr(attrmap);

    // Initialize to white on black.
    memset(attrmap, 0, dpinfo.num_text_cols * dpinfo.num_text_rows);

    // Parse out configuration bundle.
    ParseConfig(vbdptr);

    RegisterDevdata();

    // We don't really parse our VBD yet, but just for future usage.
    callbacks->MapMemVmr(vbdptr, 0);
    callbacks->LoadVmr(0);

    // Clear the screen ready for 123.
    ClearRegionForeground(dpinfo.num_text_cols,
                          dpinfo.num_text_rows,
                          0);

    return 0;
}

int __pascal DriverTerminate()
{
    traceent("DriverTerminate");

    // Clean up the screen.
    MoveCursor(0, 0);

    ClearRegionForeground(dpinfo.num_text_cols,
                          dpinfo.num_text_rows,
                          0);

    // FIXME: I thought this was correct, but it causes {SYSTEM} to crash.
    //        I need to better understand this cleanup code.
    //
    // Free our attribute map.
    //callbacks->Free(attrmap, 0, dpinfo.num_text_cols * dpinfo.num_text_rows);

    // Release any segment descriptors.
    //callbacks->FreeDescriptor(fbseg);
    //callbacks->FreeDescriptor(vbseg);

    // Release DEVDATA
    callbacks->UnregisterDevdata(devdatahdl);

    // If we were logging, close it.
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

    curx = col;
    cury = line;

    return;
}


// Write string str to the current cursor position, str is not a lmbcs
// string, just a literal sequence of bytes to write. It might not be NUL
// terminated.
//
// len is the number of bytes to write, I don't think we have to worry about
// linewrap, 123 will never ask us to write past the right column.
//
// attrs is a 6bit value, 3bits referring to the BG and three bits referring
// to the FG, these are defined by Lotus and must be translated into CGA.
//
// Mask is required because sometimes Lotus wants us to update the FG, but
// leave the BG intact, so in that case you would use MASK_FG. It's possible
// both, none or either are specified.
int WriteStringToFramebuffer(unsigned char far *str,
                             unsigned len,
                             unsigned char attrs,
                             unsigned char mask)
{
    unsigned char far *line;
    unsigned char far *attr;
    unsigned i;

    trace("WriteStringToFramebuffer");
    tracestrlen(str, len);
    traceint(len);
    traceptr(str);
    traceint(curx);
    traceint(cury);

    // It's easier to parse lotus attributes in octal, because they're
    // three bit values.
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
    }

    return curx;
}

int __pascal WriteLmbcsStringWithAttributes(unsigned byteslen,
                                            unsigned char far *lmbcsptr,
                                            unsigned char attrs)
{
    unsigned char dest[MAX_COLS];
    int result;

    traceent("WriteLmbcsStringWithAttributes");
    traceint(byteslen);
    tracestrlen(lmbcsptr, byteslen);
    traceval("%03ho", attrs);

    result = translate_lmbcs(lmbcsptr, dest, sizeof dest, byteslen, lmbcsgroup);

    WriteStringToFramebuffer(dest, result, MKFG(attrs), ATTR_FG);

    traceint(result);

    return result;
}

int __pascal WritePaddedLmbcsStringWithAttributes(unsigned byteslen,
                                                  char far *lmbcsptr,
                                                  unsigned char attrs,
                                                  unsigned startpad,
                                                  unsigned endpad)
{
    unsigned char dest[MAX_COLS];
    int result;
    traceent("WritePaddedLmbcsStringWithAttributes");
    traceint(byteslen);
    tracestrlen(lmbcsptr, byteslen);
    traceval("%03ho", attrs);
    traceint(startpad);
    traceint(endpad);

    memset(dest, ' ', sizeof dest);

    result = translate_lmbcs(lmbcsptr,
                             dest + startpad,
                             sizeof(dest) - startpad,
                             byteslen,
                             lmbcsgroup);

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

void __pascal SetRegionBgAttributes(unsigned cols, unsigned lines, char attrs)
{
    unsigned y;
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

void __pascal ClearRegionForeground(unsigned cols,
                                    unsigned lines,
                                    unsigned char attrs)
{
    unsigned y;
    unsigned origx;
    unsigned origy;
    traceent("ClearRegionForeground");

    traceint(cols);
    traceint(lines);

    traceint(curx);
    traceint(cury);

    traceval("%03ho", attrs);

    origx = curx;
    origy = cury;
    for (y = 0; y <= lines; y++) {
        WriteStringToFramebuffer(blanks, cols, MKBG(attrs), ATTR_ALL);
        curx = origx;
        cury = origy + y;
    }

    curx = origx;
    cury = origy;

    return;
}

void __pascal ClearRegionForegroundKeepBg(unsigned cols, unsigned lines)
{
    unsigned y;
    unsigned origx;
    unsigned origy;

    traceent("ClearRegionForegroundKeepBg");

    traceint(cols);
    traceint(lines);

    traceint(curx);
    traceint(cury);

    origx = curx;
    origy = cury;
    for (y = 0; y <= lines; y++) {
        WriteStringToFramebuffer(blanks, cols, 0000, ATTR_FG);
        curx = origx;
        cury = origy + y;
    }

    curx = origx;
    cury = origy;

    return;
}

// Just for debugging purposes, show where a square is on the screen.
static void DrawSquare(int topx, int topy, int bottomx, int bottomy, char c)
{
    unsigned char line[MAX_COLS];
    unsigned height;
    unsigned width;
    unsigned i;

    height = bottomy - topy;
    width  = bottomx - topx;

    memset(line, c, sizeof line);

    MoveCursor(topx, topy);
    WriteStringToFramebuffer(line, width, 7, ATTR_ALL);

    MoveCursor(topx, bottomy);
    WriteStringToFramebuffer(line, width, 7, ATTR_ALL);

    for (i = 0; i < height; i++) {
        MoveCursor(topx, topy + i);
        WriteStringToFramebuffer(line, 1, 7, ATTR_ALL);
        MoveCursor(topx + width, topy + i);
        WriteStringToFramebuffer(line, 1, 7, ATTR_ALL);
    }

    return;
}

// Move a rectangular block from the current cursor, including attributes.
//
// The region starts at curx,cury and the size is given by witdh and height.
//
// The source and destination region might overlap, and we have to handle that.
void __pascal BlockRegionCopy(int width, int height, int dstx, int dsty)
{
    unsigned char far *srcline;
    unsigned char far *dstline;
    int i;

    traceent("BlockRegionCopy");
    traceint(width);
    traceint(height);
    traceint(dstx);
    traceint(dsty);
    traceint(curx);
    traceint(cury);

    // Use this to show the source for debugging.
    //DrawSquare(curx, cury, curx + width, cury + height, '#');

    // Recall that regions can overlap, so we need to be careful about
    // our start position so we don't clobber a part we haven't processed
    // yet.
    // If we're moving a region *up* we want to start at the bottom.
    // If we're moving a region *down* we want to start at top.
    //
    // It doesn't matter if it's moving sideways, that's memmoves problem :)
    //
    // Note: I am not sure I have this logic correct.
    if (dsty <= cury)  {
        trace("region vertical direction is down or sideways");
        // We're moving down, start at the top.
        for (i = 0; i < height; i++) {
            srcline = (framebuffer + (cury + i) * dpinfo.num_text_cols * 2);
            dstline = (framebuffer + (dsty + i) * dpinfo.num_text_cols * 2);

            memmove(&dstline[dstx * 2], &srcline[curx * 2], width * 2);
        }
    } else {
        trace("region vertical direction is up");
        // We're moving up, start at the bottom.
        for (i = height - 1; i >= 0; i--) {
            srcline = (framebuffer + (cury + i) * dpinfo.num_text_cols * 2);
            dstline = (framebuffer + (dsty + i) * dpinfo.num_text_cols * 2);

            memmove(&dstline[dstx * 2], &srcline[curx * 2], width * 2);
        }
    }

    // I can't get this damn thing aligned properly >:(
    #include "padding.h"

    return;
}

int __pascal CalcSizeTranslatedString(int argstrlen, char far *argstr)
{
    int result;
    traceent("CalcSizeTranslatedString");
    tracestrlen(argstr, argstrlen);
    traceptr(argstr);

    result = translate_lmbcs(argstr, NULL, 0, argstrlen, lmbcsgroup);

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

    result = translate_lmbcs(strarg, NULL, 0, strarglen, lmbcsgroup);

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
