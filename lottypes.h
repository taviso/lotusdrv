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

struct RESDATA
{
  int rfield_0;
  int rescolumns;
  int rfield_4;
  int resrows;
  int rfield_8;
  int rfield_A;
};

struct FONTINFO
{
  char name[9];
  int angle;
  int em_pix_hgt;
  int em_pix_wid;
};


#endif
