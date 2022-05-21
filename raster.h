#ifndef __RASTER_H
#define __RASTER_H

int decode_raster_ops(char far *startaddr, char far *endaddr);

// I think these are the rasterizer opcode names, I pulled them out of the
// symbols from the SysV UNIX port of 1-2-3. There may be mistakes, I haven't
// verified they are all correct.
enum {
    RASTER_OP_JMP = 0,
    RASTER_OP_NOP,
    RASTER_OP_SETLINECOLOR,
    RASTER_OP_SETLINEPATTERN,
    RASTER_OP_SETLINEWIDTH,
    RASTER_OP_SETFILLCOLOR,
    RASTER_OP_SETFILLPATTERN,
    RASTER_OP_UNDEFINED,
    RASTER_OP_SETTEXTSIZE,
    RASTER_OP_SETTEXTALIGN,
    RASTER_OP_SETTEXTANGLE,
    RASTER_OP_SETTEXTSTYLE,
    RASTER_OP_SETMARKERSIZE,
    RASTER_OP_SETMARKERSYMBOL,
    RASTER_OP_CLEARRECTANGLE,
    RASTER_OP_DRAWSINGLELINE,
    RASTER_OP_DRAWRECTANGLEEDGES,
    RASTER_OP_DRAWRECTANGLEINTERIOR,
    RASTER_OP_DRAWRECTANGLE,
    RASTER_OP_DRAWSLICEEDGES,
    RASTER_OP_DRAWSLICEINTERIOR,
    RASTER_OP_DRAWSLICE,
    RASTER_OP_DRAWARC,
    RASTER_OP_DRAWPOLYLINE,
    RASTER_OP_DRAWPOLYGONINTERIOR,
    RASTER_OP_DRAWPOLYGON,
    RASTER_OP_DRAWTEXT,
    RASTER_OP_DRAWMARKER,
    RASTER_OP_NUMOPS,
};

// The operands to a DRAWTEXT instruction.
struct DRAWTEXT {
    int xpos;
    int ypos;
    int len;
    char far *str;
};

#endif
