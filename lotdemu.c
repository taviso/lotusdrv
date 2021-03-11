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
#include "caca.h"
#include "draw.h"

// The current cursor position.
static int curx;
static int cury;

// Selectors we need to release on exit.
unsigned fbseg;
unsigned vbseg;

// This will point into the CGA framebuffer.
static unsigned char far * framebuffer;

// These two pointers reference values in the BDA (BIOS Data Area).
static unsigned int far * bdcols;
static unsigned char far * bdrows;

// There is an "optimized" LMBCS encoding that skips the group byte
// and just uses a default value. 123 tells us what the default should
// be at startup.
static unsigned char lmbcsgroup;

// A handle returned when registering DEVDATA.
static unsigned devdatahdl;

static unsigned char blanks[MAX_COLS];

// This will point at fixed (not vmr) memory. vmrs are 123's
// abstraction of XMS/EMS handles, they need to be mapped before
// you can touch them.
static unsigned char far *attrmap;

// Not sure what this ptr is yet.
static void far *resdata;

static struct DISPLAYINFO dpinfo = {
    80,     // num_text_cols
    25,     // num_text_rows
    true,   // graphics
    true,   // full_screen_graph
    1,      // hpu_per_col
    320,    // graph_cols
    200,    // graph_rows
    1,      // graph_col_res
    1,      // graph_row_res
    100,    // view_set_size
    true,   // iscolor
};

struct FONTINFO fontinfo = {
    { 'd', 'e', 'f', 'a', 'u', 'l', 't' },
    0,  // angle
    1,  // em_pix_hgt
    1,  // em_pix_wid
};

static unsigned char fillpatts[] = {
  0x18, 0x24, 0x42, 0x81, 0x81, 0x42, 0x24, 0x18, 0x09, 0x12, 0x24, 0x48,
  0x90, 0x21, 0x42, 0x84, 0x01, 0x24, 0x02, 0x48, 0x04, 0x90, 0x09, 0x20,
  0x12, 0x40, 0x24, 0x80, 0x49, 0x00, 0x92, 0x00, 0x24, 0x04, 0x48, 0x08,
  0x90, 0x10, 0x20, 0x24, 0x40, 0x48, 0x80, 0x90, 0x01, 0x80, 0x02, 0x40,
  0x04, 0x20, 0x08, 0x10, 0x10, 0x08, 0x20, 0x04, 0x40, 0x02, 0x80, 0x01,
  0x80, 0x01, 0x40, 0x02, 0x20, 0x04, 0x10, 0x08, 0x08, 0x10, 0x04, 0x20,
  0x02, 0x40, 0x01, 0x80, 0x00, 0x09, 0x00, 0x12, 0x00, 0x24, 0x00, 0x48,
  0x00, 0x90, 0x01, 0x20, 0x02, 0x40, 0x04, 0x80, 0x09, 0x00, 0x12, 0x00,
  0x24, 0x00, 0x48, 0x00, 0x90, 0x00, 0x20, 0x01, 0x40, 0x02, 0x80, 0x04,
  0x00, 0x04, 0x00, 0x08, 0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x00, 0x80,
  0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08, 0x00, 0x10, 0x00, 0x20, 0x00,
  0x40, 0x00, 0x80, 0x00, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff, 0x55, 0x55,
  0xee, 0xee, 0x55, 0x55, 0xbb, 0xbb, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55,
  0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x44, 0x44, 0xaa, 0xaa, 0x11, 0x11,
  0xaa, 0xaa, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00, 0x88, 0x88, 0x00, 0x00,
  0x22, 0x22, 0x00, 0x00
};

// I think an mu must be a "movement unit"?
static struct DEVDATA devdata = {
  1,        // ShowMeFlag
  200,      // RasterHeight
  1,        // AspectX
  1,        // AspectY
  1,        // RHmusPerTHmu
  1,        // RHmuPerGHmus
  1,        // RVmusPerTVmu
  1,        // RVmuPerGVmus
  {
      { { 8,  8  }, { 0, 0 }, &fillpatts[  0], { 0, 0 } },  // 0
      { { 8,  8  }, { 0, 0 }, &fillpatts[  0], { 0, 0 } },  // 1
      { { 14, 14 }, { 0, 0 }, &fillpatts[  0], { 0, 0 } },  // 2
      { { 16, 16 }, { 0, 0 }, &fillpatts[  0], { 0, 0 } },  // 3
      { { 16, 16 }, { 0, 0 }, &fillpatts[  0], { 0, 0 } },  // 4
      { { 14, 14 }, { 0, 0 }, &fillpatts[  0], { 0, 0 } },  // 5
      { {  4, 16 }, { 0, 0 }, &fillpatts[  0], { 0, 0 } },  // 6
      { {  4, 16 }, { 0, 0 }, &fillpatts[  0], { 0, 0 } },  // 7
      { {  4, 16 }, { 0, 0 }, &fillpatts[  0], { 0, 0 } },  // 8
      { {  4, 16 }, { 0, 0 }, &fillpatts[  0], { 0, 0 } },  // 9
      { {  4, 16 }, { 0, 0 }, &fillpatts[  0], { 0, 0 } },  // 10
      { {  4, 16 }, { 0, 0 }, &fillpatts[  0], { 0, 0 } },  // 11
  },        // FillPatts
  {
    CACA_MAGENTA,   CACA_MAGENTA,   CACA_CYAN,  CACA_BLACK,
    CACA_MAGENTA,   CACA_CYAN,      CACA_BLACK, CACA_BLACK,
    CACA_WHITE,     CACA_MAGENTA,   CACA_CYAN,  CACA_BLACK,
    CACA_MAGENTA,   CACA_CYAN,      CACA_WHITE, CACA_BLACK,
  },        // ColorMap
  {
    0xFFFF, 0xAAAA, 0xCCCC, 0xCACA,
    0xC0C0, 0x00FF, 0xFF24, 0x0000
  },        // SrcPatD
  {
    0x0001, 0x0006, 0x000C, 0x0012,
    0x0018, 0x001E, 0x0024, 0x002A
  },        // SrcPatS
  26,       // FIndexCnt
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ",     // FontIndex
  1,            // FNamesCnt
  &fontinfo,    // FontNames
  {
    exprt_scan_linx,
    exprt_fill_rect,
    exprt_thin_diag_line,
    exprt_thin_vert_line,
    exprt_shade_rect,
    exprt_fill_scan_list,
  },        // DevFuncs
};

struct LOTUSFUNCS far *callbacks;

// The canvas used for drawing ascii-art graphics.
caca_canvas_t *cv;

void __pascal GetDisplayInfo(struct DISPLAYINFO far *dspinfo)
{
    traceent("GetDisplayInfo");
    traceptr(dspinfo);

    memcpy(dspinfo, &dpinfo, sizeof dpinfo);

    tracebuf(dspinfo, sizeof dpinfo);

    return;
}


static int RegisterDevdata()
{
    int result;
    void *destptr;
    void *vmrptr;

    trace("registering DEVDATA");

    tracebuf(&devdata, sizeof devdata);

    // Allocate a VMR buffer to give to 123.
    destptr = callbacks->AllocMem(0x27, sizeof devdata, 1);

    traceptr(destptr);

    // Copy out buffer over.
    memcpy(destptr, &devdata, sizeof devdata);

    vmrptr = callbacks->LoadVmr(1);

    traceptr(vmrptr);

    // Register it, we release this handle in our DriverTerminate().
    devdatahdl = callbacks->RegisterDevdataStruct(vmrptr, &result);

    traceint(devdatahdl);
    traceint(result);
    return 0;
}

int __pascal DriverInit(void far *drvapi,
                        struct BDLHDR far *vbdptr,
                        const char far *cfgname,
                        char deflmbcsgrp)
{
    // If requested, log all calls
    openlog(DEBUG_LOGFILE);

    traceent("DriverInit");
    traceptr(drvapi);
    traceptr(vbdptr);
    tracestr(cfgname);
    tracechr(deflmbcsgrp);

    // Save this pointer for calling lotus functions.
    callbacks = drvapi;

    // Descriptors we want installed.
    vbseg = BIOS_DATA_AREA;
    fbseg = CGA_FRAMEBUF;

    tracebuf(callbacks, sizeof *callbacks);

    // Record the default lmbcs group
    lmbcsgroup = deflmbcsgrp;

    // Create a line of blanks for quickly clearing a row.
    memset(blanks, ' ', sizeof blanks);

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

    dpinfo.graph_cols = dpinfo.num_text_cols;
    dpinfo.graph_rows = dpinfo.num_text_rows;
    devdata.RasterHeight = dpinfo.graph_rows;
    devdata.AspectX = 2;
    devdata.AspectY = 1;

    traceint(dpinfo.num_text_cols);
    traceint(dpinfo.num_text_rows);

    // Request an LDT for framebuffer access.
    callbacks->GetDescriptor(&fbseg,
                             0,
                             dpinfo.num_text_cols * dpinfo.num_text_rows * 2);

    traceint(fbseg);

    framebuffer = MK_FP(fbseg, 0);

    // Dump a few bytes just to make sure this look sane.
    tracebuf(framebuffer, 32);

    // For some reason the order of these operations is important, otherwise
    // DEVDATA gets unmapped at inconvenient times.
    RegisterDevdata();

    // Setup the CGA <=> 123 attribute map.
    attrmap = callbacks->AllocMem(0x27,
                                  dpinfo.num_text_cols * dpinfo.num_text_rows,
                                  0);
    //attrmap = callbacks->Allocate(dpinfo.num_text_cols * dpinfo.num_text_rows, 0);
    //callbacks->LoadVmr(0);

    // Out of memory?
    if (attrmap == NULL) {
        logmsg("failed to allocate space for the attribute map");
        return 1;
    }

    traceptr(attrmap);

    // Initialize to white on black.
    memset(attrmap, 0, dpinfo.num_text_cols * dpinfo.num_text_rows);

    // Parse out configuration bundle.
    ParseConfig(vbdptr);

    // Clear the screen ready for 123.
    ClearRegionForeground(dpinfo.num_text_cols,
                          dpinfo.num_text_rows,
                          0);

    // Initialize a canvas for ASCII-art graphics.
    cv = caca_create_canvas(dpinfo.num_text_cols,
                            dpinfo.num_text_rows,
                            fbseg,
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

    // Free our attribute map.
    callbacks->Free(attrmap, 0, dpinfo.num_text_cols * dpinfo.num_text_rows);

    // Release any segment descriptors.
    callbacks->FreeDescriptor(vbseg);
    callbacks->FreeDescriptor(fbseg);

    // Release DEVDATA.
    callbacks->UnregisterDevdata(devdatahdl);

    // Decrement canvas reference count.
    caca_free_canvas(cv);

    // If we were logging, close it.
    //closelog();

    return 0;
}

// This is called when 1-2-3 exits graphics mode.
int __pascal ResetDisplay()
{
    traceent("ResetDisplay");
    MoveCursor(0, 0);
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

    for (y = 0; y <= lines; y++) {
        WriteStringToFramebuffer(NULL, cols, MKBG(attrs), ATTR_BG);
        MoveCursor(origx, origy + y);
    }

    MoveCursor(origx, origy);

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
        MoveCursor(origx, origy + y);
    }

    MoveCursor(origx, origy);

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
        MoveCursor(origx, origy + y);
    }

    MoveCursor(origx, origy);

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
    traceent("FitTranslatedString");
    traceint(strarglen);
    tracestrlen(strarg, strarglen);
    traceint(ncols);
    traceptr(bytesneeded);

    *bytesneeded = translate_lmbcs(strarg, NULL, 0, strarglen, lmbcsgroup);

    traceint(*bytesneeded);

    return *bytesneeded;
}

void __pascal SetCursorInvisible()
{
    traceent("SetCursorInvisible");

    // https://stanislavs.org/helppc/int_10-1.html
    //
    // To hide the cursor, you simply set bit 5 of ch. int 10,1
    // supports configuring the size of the cursor too, but that
    // doesn't work in terminal mode - it's all or nothing.
    __asm {
        mov ah, 1
        mov ch, 1 << 5
        mov cl, 0
        int 0x10
    }
    return;
}

void __pascal SetCursorVisible()
{
    traceent("SetCursorVisible");

    // First we make sure the "hide cursor" bit isn't set, see the
    // implementation of SetCursorInvisible() for details.
    __asm {
        mov ah, 1
        mov ch, 0
        mov cl, 0
        int 0x10
    }

    // Now we need to put it in the right place, you can set the position with
    // int 10,2. https://stanislavs.org/helppc/int_10-2.html
    __asm {
        mov ah, 2
        mov bx, cury
        mov cx, curx
        mov dl, cl
        mov dh, bl
        int 0x10
    }
    return;
}

void __pascal LockCursorAttributes(int unused, int cursmodenum)
{
    traceent("LockCursorAttributes");
    traceint(unused);
    traceint(cursmodenum);
    return;
}

int __pascal QQCalledBeforeGraphicsMode(void far *m)
{
    int result;
    struct RESDATA r;

    traceent("QQCalledBeforeGraphicsMode");
    traceptr(m);
    tracebuf(m, 16);
    // I don't understand this function, I'm just duplicating
    // the logic from the real CGA driver.
    callbacks->MapMemVmr(m, 0);
    callbacks->LoadVmr(0);

    memcpy(&r, (char far *)(m) + 0x15, sizeof r);

    callbacks->field_58(devdatahdl, 0);
    result = callbacks->field_5C(devdatahdl, &r);

    callbacks->MapMemVmr(m, 0);
    callbacks->LoadVmr(0);
    return 0;
}

int __pascal QQLotusApiJustCallsFunc_1(int strarglen, void far *strarg)
{
    traceent("QQLotusApiJustCallsFunc_1");
    traceint(strarglen);
    traceptr(strarg);
    tracebuf(strarg, strarglen);
    return callbacks->field_68(devdatahdl, strarglen, strarg, resdata);
}

void __pascal MoveCursor2(int col, int line)
{
    traceent("MoveCursor2");
    MoveCursor(col, line);
    return;
}

int __pascal IsGraphicsModeReady(void far *ptr)
{
    int result;
    traceent("IsGraphicsModeReady");
    traceptr(ptr);
    tracebuf(ptr, 16);
    traceint(devdatahdl);
    traceptr(cv);
    result = callbacks->field_64(devdatahdl, ptr);
    traceint(result);
    tracebuf(ptr, 16);
    return true;
}

int __pascal ComplicatedNop()
{
    traceent("ComplicatedNop");
    return 0;
}

int __pascal zerosub_0()
{
    traceent("zerosub_0");
    return 0;
}

int __pascal LotusApiPassedPtr(void far *ptr)
{
    traceent("LotusApiPassedPtr");
    traceptr(ptr);
    resdata = ptr;
    return 0;
}

int __pascal DrawStringAtPosition(int len, char far *str, int c, void far *d)
{
    traceent("DrawStringAtPosition");
    traceint(len);
    traceptr(str);
    tracestrlen(str, len);
    traceval("%#x", c);
    traceptr(d);
    tracebuf(d, 32);
    return callbacks->field_6C(devdatahdl, len, str, c, d, resdata);
}

int __pascal nullsub_5(int a, int b)
{
    traceent("nullsub_5");
    traceptr(a);
    traceptr(b);
    return 0;
}
