#ifndef __DRAW_H
#define __DRAW_H

extern void __pascal exprt_scan_linx(int a, int b, int c, int d);
extern void __pascal exprt_fill_rect(int a, int b, int c, int d, int e);
extern void __pascal exprt_thin_diag_line(int a, int b, int c, int d, int e);
extern void __pascal exprt_thin_vert_line(int a, int b, int c, int d);
extern void __pascal exprt_shade_rect(int a, int b, int c, int d, void far *e, int f);
extern void __pascal exprt_fill_scan_list();

extern caca_canvas_t *cv;

#endif
