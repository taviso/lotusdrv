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
#include "raster.h"

// You can either disable labels on graphs, try to draw them graphically, or
// try to draw them as text.
// If you set WANT_GRAPH_LABELS, the 1-2-3 rasterizer will try to drawn them.
// This is unlikely to look good at low resolutions.
// If you set WANT_TEXT_LABELS, then we will try to parse the rasterizer
// bytecode to figure out where the labels are, and then draw them ourselves.
// If you set neither, no labels will get drawn.
#define WANT_GRAPH_LABELS 0     // Note: not working yet
#define WANT_TEXT_LABELS 0      // Note: nearly working!

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

// A handle returned when opening the rasterizer.
static unsigned rasthdl;

static unsigned char blanks[MAX_COLS];

// This will point at fixed (not vmr) memory. vmrs are 123's
// abstraction of XMS/EMS handles, they need to be mapped before
// you can touch them.
static unsigned char far *attrmap;

// Not sure what this ptr is yet.
static void far *curview;

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

void __pascal drv_disp_info(struct DISPLAYINFO far *dspinfo)
{
    traceent("drv_disp_info");
    traceptr(dspinfo);

    memcpy(dspinfo, &dpinfo, sizeof dpinfo);

    tracebuf(dspinfo, sizeof dpinfo);

    return;
}


static int start_lotus_rasterizer()
{
    int result;
    void *destptr;
    void *vmrptr;

    trace("starting rasterizer...");

    tracebuf(&devdata, sizeof devdata);

    // Allocate a VMR buffer to give to 123.
    destptr = callbacks->alloc_mptr(0x27, sizeof devdata, 1);

    traceptr(destptr);

    // Copy out buffer over.
    memcpy(destptr, &devdata, sizeof devdata);

    vmrptr = callbacks->drv_get_vmr(1);

    traceptr(vmrptr);

    // Register it, we release this handle in our drv_disp_close().
    rasthdl = callbacks->open_rasterizer(vmrptr, &result);

    traceint(rasthdl);
    traceint(result);
    return 0;
}

int __pascal drv_disp_open(void far *drvapi,
                           struct BDLHDR far *vbdptr,
                           const char far *cfgname,
                           char deflmbcsgrp)
{
    // If requested, log all calls
    openlog(DEBUG_LOGFILE);

    traceent("drv_disp_open");
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
    callbacks->set_disp_buf(&vbseg, 0, 0x100);

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
    callbacks->set_disp_buf(&fbseg,
                             0,
                             dpinfo.num_text_cols * dpinfo.num_text_rows * 2);

    traceint(fbseg);

    framebuffer = MK_FP(fbseg, 0);

    // Dump a few bytes just to make sure this look sane.
    tracebuf(framebuffer, 32);

    // For some reason the order of these operations is important, otherwise
    // DEVDATA gets unmapped at inconvenient times.
    start_lotus_rasterizer();

    // Setup the CGA <=> 123 attribute map.
    attrmap = callbacks->alloc_mptr(0x27,
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
    drv_disp_clear(dpinfo.num_text_cols,
                   dpinfo.num_text_rows,
                   0);

    // Initialize a canvas for ASCII-art graphics.
    cv = caca_create_canvas(dpinfo.num_text_cols,
                            dpinfo.num_text_rows,
                            fbseg,
                            0);
    return 0;
}

int __pascal drv_disp_close()
{
    traceent("drv_disp_close");

    // Clean up the screen.
    drv_disp_set_pos(0, 0);

    drv_disp_clear(dpinfo.num_text_cols,
                   dpinfo.num_text_rows,
                   0);

    // Free our attribute map.
    callbacks->free_fixed_block(attrmap,
                                0,
                                dpinfo.num_text_cols * dpinfo.num_text_rows);

    // Release any segment descriptors.
    callbacks->drop_disp_buf(vbseg);
    callbacks->drop_disp_buf(fbseg);

    // No more rasterization needed.
    callbacks->close_rasterizer(rasthdl);

    // Decrement canvas reference count.
    caca_free_canvas(cv);

    // If we were logging, close it.
    //closelog();

    return 0;
}

// This is called when 1-2-3 exits graphics mode.
int __pascal drv_disp_text()
{
    traceent("drv_disp_text");
    drv_disp_set_pos(0, 0);
    return 1;
}

// This is called when a graph is requested (F10).
int __pascal drv_disp_graph()
{
    traceent("SetGraphicsMode");
    return 1;
}

void __pascal drv_disp_set_pos(int col, int line)
{
    traceent("drv_disp_set_pos");
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
static int WriteStringToFramebuffer(unsigned char far *str,
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

int __pascal drv_disp_write(unsigned byteslen,
                                unsigned char far *lmbcsptr,
                                unsigned char attrs)
{
    unsigned char dest[MAX_COLS];
    int result;

    traceent("drv_disp_write");
    traceint(byteslen);
    tracestrlen(lmbcsptr, byteslen);
    traceval("%03ho", attrs);

    result = translate_lmbcs(lmbcsptr, dest, sizeof dest, byteslen, lmbcsgroup);

    WriteStringToFramebuffer(dest, result, MKFG(attrs), ATTR_FG);

    traceint(result);

    return result;
}

int __pascal drv_disp_zone(unsigned byteslen,
                           char far *lmbcsptr,
                           unsigned char attrs,
                           unsigned startpad,
                           unsigned endpad)
{
    unsigned char dest[MAX_COLS];
    int result;
    traceent("drv_disp_zone");
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

void __pascal drv_disp_set_bg(unsigned cols, unsigned lines, char attrs)
{
    unsigned y;
    int origx;
    int origy;
    traceent("drv_disp_set_bg");

    traceint(cols);
    traceint(lines);

    traceint(curx);
    traceint(cury);

    traceval("%03ho", attrs);

    origx = curx;
    origy = cury;

    for (y = 0; y <= lines; y++) {
        WriteStringToFramebuffer(NULL, cols, MKBG(attrs), ATTR_BG);
        drv_disp_set_pos(origx, origy + y);
    }

    drv_disp_set_pos(origx, origy);

    return;
}

void __pascal drv_disp_clear(unsigned cols,
                             unsigned lines,
                             unsigned char attrs)
{
    unsigned y;
    unsigned origx;
    unsigned origy;
    traceent("drv_disp_clear");

    traceint(cols);
    traceint(lines);

    traceint(curx);
    traceint(cury);

    traceval("%03ho", attrs);

    origx = curx;
    origy = cury;

    for (y = 0; y <= lines; y++) {
        WriteStringToFramebuffer(blanks, cols, MKBG(attrs), ATTR_ALL);
        drv_disp_set_pos(origx, origy + y);
    }

    drv_disp_set_pos(origx, origy);

    return;
}

void __pascal drv_disp_fg_clear(unsigned cols, unsigned lines)
{
    unsigned y;
    unsigned origx;
    unsigned origy;

    traceent("drv_disp_fg_clear");

    traceint(cols);
    traceint(lines);

    traceint(curx);
    traceint(cury);

    origx = curx;
    origy = cury;
    for (y = 0; y <= lines; y++) {
        WriteStringToFramebuffer(blanks, cols, 0000, ATTR_FG);
        drv_disp_set_pos(origx, origy + y);
    }

    drv_disp_set_pos(origx, origy);

    return;
}

#ifndef RELEASE
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

    drv_disp_set_pos(topx, topy);
    WriteStringToFramebuffer(line, width, 7, ATTR_ALL);

    drv_disp_set_pos(topx, bottomy);
    WriteStringToFramebuffer(line, width, 7, ATTR_ALL);

    for (i = 0; i < height; i++) {
        drv_disp_set_pos(topx, topy + i);
        WriteStringToFramebuffer(line, 1, 7, ATTR_ALL);
        drv_disp_set_pos(topx + width, topy + i);
        WriteStringToFramebuffer(line, 1, 7, ATTR_ALL);
    }

    return;
}
#endif

// Move a rectangular block from the current cursor, including attributes.
//
// The region starts at curx,cury and the size is given by width and height.
//
// The source and destination region might overlap, and we have to handle that.
void __pascal drv_disp_copy(int width, int height, int dstx, int dsty)
{
    unsigned char far *srcline;
    unsigned char far *srcattr;
    unsigned char far *dstline;
    unsigned char far *dstattr;
    int i;

    traceent("drv_disp_copy");
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
            srcattr = (attrmap + (cury + i) * dpinfo.num_text_cols * 1);
            dstline = (framebuffer + (dsty + i) * dpinfo.num_text_cols * 2);
            dstattr = (attrmap + (dsty + i) * dpinfo.num_text_cols * 1);

            memmove(&dstline[dstx * 2], &srcline[curx * 2], width * 2);
            memmove(&dstattr[dstx], &srcattr[curx], width);
        }
    } else {
        trace("region vertical direction is up");
        // We're moving up, start at the bottom.
        for (i = height - 1; i >= 0; i--) {
            srcline = (framebuffer + (cury + i) * dpinfo.num_text_cols * 2);
            srcattr = (attrmap + (cury + i) * dpinfo.num_text_cols * 1);
            dstline = (framebuffer + (dsty + i) * dpinfo.num_text_cols * 2);
            dstattr = (attrmap + (dsty + i) * dpinfo.num_text_cols * 1);

            memmove(&dstline[dstx * 2], &srcline[curx * 2], width * 2);
            memmove(&dstattr[dstx], &srcattr[curx], width);
        }
    }

    return;
}

int __pascal drv_disp_size(int argstrlen, char far *argstr)
{
    int result;
    traceent("drv_disp_size");
    tracestrlen(argstr, argstrlen);
    traceptr(argstr);

    result = translate_lmbcs(argstr, NULL, 0, argstrlen, lmbcsgroup);

    traceint(result);
    return result;
}

int __pascal drv_disp_fit(int strarglen,
                              char far *strarg,
                              int ncols,
                              int far *bytesneeded)
{
    traceent("drv_disp_fit");
    traceint(strarglen);
    tracestrlen(strarg, strarglen);
    traceint(ncols);
    traceptr(bytesneeded);

    *bytesneeded = translate_lmbcs(strarg, NULL, 0, strarglen, lmbcsgroup);

    traceint(*bytesneeded);

    // Truncate if necessary.
    return *bytesneeded = __min(*bytesneeded, ncols);
}

void __pascal drv_disp_curs_off()
{
    traceent("drv_disp_curs_off");

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

void __pascal drv_disp_curs_on()
{
    traceent("drv_disp_curs_on");

    // First we make sure the "hide cursor" bit isn't set, see the
    // implementation of drv_disp_curs_off() for details.
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

// I'm not sure what I'm supposed to do here.
void __pascal drv_disp_curs_type(int unused, int cursmodenum)
{
    traceent("drv_disp_curs_type");
    traceint(unused);
    traceint(cursmodenum);
    return;
}

#if WANT_GRAPH_LABELS && WANT_TEXT_LABELS
# error you cant have graphical and text labels on graphs, choose one.
#endif

// Lotus calls this just before it sends bytecode to the rasterizer.
//
// The interesting thing is we have an opportunity to parse
// the bytecode and patch it, which could let us extract labels and
// paint them directly on graphs instead of trying to rasterize them.
int __pascal drv_disp_grph_process(struct GRAPH far *graph)
{
    void far *entry;
    void far *exit;
    int result;
    struct RASTEROPS rops;

    traceent("drv_disp_grph_process");

    traceptr(graph);
    tracebuf(graph, 64);

    callbacks->drv_map_mptr(graph, 0);
    graph = callbacks->drv_get_vmr(0);

    memcpy(&rops, &graph->rops, sizeof rops);

    traceptr(rops.screeninfo);      // contains fontdata, resolution, etc.
    traceptr(rops.bytecode);        // Rasterizer bytecode
    tracebuf(rops.bytecode, 32);
    traceint(rops.rfield_8);        // Boolean Flag, no ops if zero
    traceint(rops.rfield_A);        // Some ops do change it (inc or dec), dunno what it is.

    callbacks->set_strip(rasthdl, 0);

    // Record the bytecode entrypoint.
    entry = rops.bytecode;

    // Run the rasterizer.
    result = callbacks->raster(rasthdl, &rops);

    traceint(result);

    // Now we know where the bytecode ends.
    exit = rops.bytecode;

#if WANT_TEXT_LABELS
    // If bytecode was executed, we can trace through it and locate and
    // extracts labels. That's useful, because otherwise we have rasterized
    // labels which dont look good on a terminal because they're tiny little
    // lines that can't be drawn accurately with libcaca.
    if (result) {
        int opcnt = decode_raster_ops(entry, exit);
        traceint(opcnt);
    }
#endif

    callbacks->drv_map_mptr(graph, 0);
    graph = callbacks->drv_get_vmr(0);

    // Copy the results over.
    memcpy(&graph->rops, &rops, sizeof rops);

    // Dump the next bytecode instructions.
    traceptr(rops.bytecode);
    tracebuf(rops.bytecode, 32);

    return result;
}

int __pascal drv_disp_grph_txt_size(int strarglen, void far *strarg)
{
    int result = 0;
    traceent("drv_disp_grph_txt_size");
    traceint(strarglen);
    traceptr(strarg);
    tracebuf(strarg, strarglen);
#if WANT_GRAPH_LABELS
    result = callbacks->rast_txt_size(rasthdl, strarglen, strarg, curview);
#elif WANT_TEXT_LABELS
    // XXX: See the discssion in drv_disp_grph_txt_fit.
    result = strarglen;
#endif
    traceint(result);
    return result;
}

void __pascal drv_disp_set_pos_hpu(int col, int line)
{
    traceent("drv_disp_set_pos_hpu");
    drv_disp_set_pos(col, line);
    return;
}

int __pascal drv_disp_grph_compute_view(void far *ptr)
{
    int result;
    traceent("drv_disp_grph_compute_view");
    traceptr(ptr);
    tracebuf(ptr, 16);
    traceint(rasthdl);
    traceptr(cv);
    result = callbacks->rast_compute_view(rasthdl, ptr);
    traceint(result);
    tracebuf(ptr, 16);
    return true;
}

int __pascal drv_disp_unlock()
{
    traceent("drv_disp_unlock");
    return 0;
}

int __pascal drv_disp_lock()
{
    traceent("drv_disp_lock");
    return 0;
}

int __pascal drv_disp_grph_set_cur_view(void far *view)
{
    traceent("drv_disp_grph_set_cur_view");
    traceptr(view);
    curview = view;
    return 0;
}

int __pascal drv_disp_grph_txt_fit(int len, char far *str, int maxlen, int far *used)
{
    int result;
    traceent("drv_disp_grph_txt_fit");
    traceint(len);
    traceptr(str);
    tracestrlen(str, len);
    traceval("%#x", maxlen);
    traceptr(used);

    result = *used = 0;

#if WANT_GRAPH_LABELS
    result = callbacks->rast_txt_fit(rasthdl, len, str, maxlen, used, curview);
#elif WANT_TEXT_LABELS
    // XXX: I want to tell the rasterizer how much space I need for the text,
    // but if it's vertical it takes me much more space, because the terminal
    // aspect ratio is 2:1 (pixels are taller than they are wide).
    //
    // I don't know how it wants to draw this, should I use worst or best case?
    //
    // I think I should tell it *best* case, and then I can re-do the fit
    // operation later when I'm parsing the bytecode and know the alignment and
    // angle. I think this doesn't work yet, but there are raster opcodes like
    // SetTextAngle and SetTextAlignment, so it seems plausible.
    result = *used = __min(len, maxlen);
#endif

    traceint(result);
    return result;
}

int __pascal drv_disp_sync(int a, int b)
{
    traceent("drv_disp_sync");
    traceptr(a);
    traceptr(b);
    return 0;
}
