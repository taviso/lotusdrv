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

static const char far *raster_op_name(unsigned op)
{
    char far *raster_op_names[] = {
        "Jump",
        "Nop",
        "SetLineColor",
        "SetLinePattern",
        "SetLineWidth",
        "SetFillColor",
        "SetFillPatten",
        "undefined",
        "SetTextSize",
        "SetTextAlign",
        "SetTextAngle",
        "SetTextStyle",
        "SetMarkerSize",
        "SetMarkerSymbol",
        "ClearRectangle",
        "DrawSingleLine",
        "DrawRectangleEdges",
        "DrawRectangleInterior",
        "DrawRactangle",
        "DrawSliceEdges",
        "DrawSliceInterior",
        "DrawSlice",
        "DrawArc",
        "DrawPolyLine",
        "DrawPolygonInterior",
        "DrawPolygon",
        "DrawText",
        "DrawMarker",
    };

    if (op < RASTER_OP_NUMOPS) {
        return raster_op_names[op];
    }

    return NULL;
}

int decode_raster_ops(char far *startaddr, char far *endaddr)
{
    int opcnt;
    int oplen;
    char type;
    char opcode;
    char param;
    char far *addr;
    char far *rasterop;

    trace("decode_raster_ops");
    traceptr(startaddr);
    traceptr(endaddr);

    for (opcnt = 0, rasterop = startaddr; rasterop != endaddr; opcnt++) {
        traceptr(rasterop);
        tracebuf(rasterop, 16);

        type    = *rasterop++;
        opcode  = (char)((type & 0x7f) - 1);
        param   = *rasterop++;
        addr    = *(char far **) rasterop;  // Only used for JMP?
        oplen   = 0;

        traceint(type);
        traceint(opcode);
        traceint(param);
        traceptr(addr);

        logmsg("opcode %02x %s@", opcode, raster_op_name(opcode));


        if (opcode == RASTER_OP_DRAWTEXT) {
            struct DRAWTEXT *label = (void far *) rasterop;

            traceptr(label);
            traceval("%u", label->xpos);
            traceval("%u", label->ypos);
            traceint(label->len);
            traceptr(label->str);
            tracestrlen(label->str, label->len);
        }

        // This is a jmp to a far pointer.
        if (param < 0 && opcode == RASTER_OP_JMP) {
            traceptr(addr);
            tracebuf(addr, 32);
            rasterop = addr;
            continue;
        }

        if (type < 0) {
            // The top bit indicates the instruction is variable width.
            oplen = param & 0x1f;
            // If the 1-byte length is zero, then it's a 2-byte width.
            if (oplen == 0) {
                oplen = *(int *) rasterop;
            }

            rasterop += oplen;
        }
    }
    return opcnt;
}
