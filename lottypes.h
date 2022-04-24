#ifndef __LOTTYPES_H
#define __LOTTYPES_H

#pragma pack(1)

#include "idatypes.h"

struct DISPLAYINFO
{
  int num_text_cols;
  int num_text_rows;
  int graphics;
  int full_screen_graph;
  int hpu_per_col;
  int graph_cols;
  int graph_rows;
  int graph_col_res;
  int graph_row_res;
  int view_set_size;
  int iscolor;
};

struct BDLHDR
{
  int bdllen;
  int bdlver;
  int driver_id;
  int bundle_id;
};

struct BDLRECHDR
{
  int bdltype;
  int reclen;
};

struct DEVPRIM
{
  void (__pascal __far *scan_linx)(int, int, int, int);
  void (__pascal __far *fill_rect)(int, int, int, int, int);
  void (__pascal __far *thin_diag_line)(int, int, int, int, int);
  void (__pascal __far *thin_vert_line)(int, int, int, int);
  void (__pascal __far *shade_rect)(int, int, int, int, void far *, int);
  void (__pascal __far *fill_scan_list)();
};

typedef struct DEVPRIM DEVPRIM;

struct POINT
{
  int pty;
  int ptx;
};

typedef struct POINT POINT;

struct PATT
{
  POINT pattsize;
  POINT patthotSpot;
  void far *pattptr;
  POINT pattSecOffset;
};

typedef struct PATT PATT;

struct DEVDATA
{
  int ShowMeFlag;
  int RasterHeight;
  int AspectX;
  int AspectY;
  int RHmusPerTHmu;
  int RHmuPerGHmus;
  int RVmusPerTVmu;
  int RVmuPerGVmus;
  PATT FillPatts[12];
  unsigned char ColorMap[16];
  int SrcPatD[8];
  int SrcPatS[8];
  int FIndexCnt;
  void far *FontIndex;
  int FNamesCnt;
  void far *FontNames;
  DEVPRIM DevFuncs;
};

// I'm still a bit lost on what all these fields are.
// I have no idea how 123 knows where the opcodes end.
struct GRAPH
{
    char unknown0;
    char unknown1;
    char far *screeninfo;   // Seems to contain resolution and font data.
    char far *rasterops;    // Sometimes a duplicate of the bytecode ptr?
    int unknown2;
    int unknown3;
    int unknown4;
    int unknown5;
    int unknown6;
    char unknown7;
    struct RASTEROPS {
        void far *screeninfo;
        void far *bytecode;
        int rfield_8;       // Always seems to be 1
        int rfield_A;
    } rops;
};

struct FONTINFO
{
  char name[9];
  int angle;
  int em_pix_hgt;
  int em_pix_wid;
};


#endif
