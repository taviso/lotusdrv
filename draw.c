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

// The canvas used for drawing ascii-art graphics.
extern caca_canvas_t *cv;

void __pascal exprt_scan_linx(int x, int y, int width, int attr)
{
    traceent("exprt_scan_linx");
    traceint(x);
    traceint(y);
    traceint(width);
    traceint(attr);
    caca_set_color_ansi(cv, attr, attr);
    caca_draw_line(cv, x, y, x + width, y, '-');
    return;
}

void __pascal exprt_fill_rect(int x, int y, int width, int height, int attr)
{
    traceent("exprt_fill_rect");
    traceint(x);
    traceint(y);
    traceint(width);
    traceint(height);
    traceint(attr);

    caca_set_color_ansi(cv, attr, CACA_WHITE);
    caca_fill_box(cv, x, y, width, height, ' ');
    return;
}

void __pascal exprt_thin_diag_line(int x1, int y1, int x2, int y2, int attr)
{
    traceent("exprt_thin_diag_line");
    traceint(x1);
    traceint(y1);
    traceint(x2);
    traceint(y2);
    traceint(attr);
    caca_set_color_ansi(cv, attr, attr);
    caca_draw_thin_line(cv, x1, y1, x2, y2);
    return;
}

void __pascal exprt_thin_vert_line(int x, int y, int height, int attr)
{
    traceent("exprt_thin_vert_line");
    traceint(x);
    traceint(y);
    traceint(height);
    traceint(attr);
    caca_set_color_ansi(cv, attr, attr);
    caca_draw_line(cv, x, y, x, y + height, '|');
    return;
}

void __pascal exprt_shade_rect(int a, int b, int c, int d, void far *e, int f)
{
    traceent("exprt_shade_rect");
    traceint(a);
    traceint(b);
    traceint(c);
    traceint(d);
    traceptr(e);
    traceint(f);
    return;
}

void __pascal exprt_fill_scan_list()
{
    traceent("exprt_fill_scan_list");
    return;
}

