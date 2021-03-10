#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <conio.h>
#include <dos.h>

#include "config.h"
#include "debug.h"
#include "idatypes.h"
#include "lottypes.h"
#include "lotcalls.h"

extern void __pascal __far exprt_scan_linx(int, int, int, int);
extern void __pascal __far exprt_fill_rect(int, int, int, int, int);
extern void __pascal __far exprt_thin_diag_line(int, int, int, int, int);
extern void __pascal __far exprt_thin_vert_line(int, int, int, int);
extern int  __pascal __far exprt_shade_rect(int, int, int, int, void far *, int);
extern void __pascal __far exprt_fill_scan_list();

// This is a debugging shim that logs what a real driver is doing.

static void far * __pascal _wrap_AllocMem(int a, int size, int vmr)
{
    void far *result;
    trace("callbacks->AllocMem");
    traceint(a);
    traceint(size);
    traceint(vmr);

    result = callbacks->AllocMem(a, size, vmr);

    traceptr(result);
    tracebuf(result, size);
    return result;
}

static void __pascal _wrap_lfield_8(void far *ptr, int size)
{
    trace("callbacks->lfield_8");
    traceptr(ptr);
    traceint(size);
    tracebuf(ptr, size);
    callbacks->lfield_8(ptr, size);
    tracebuf(ptr, size);
    return;
}

static void far * __pascal _wrap_Allocate(int tag, int size)
{
    void *result;
    trace("callbacks->Allocate");
    traceint(tag);
    traceint(size);
    result = callbacks->Allocate(tag, size);
    traceptr(result);
    tracebuf(result, size);
    return result;
}

static int __pascal _wrap_Free(void *addr, int unknown, int size)
{
    int result;

    trace("callbacks->Free");
    traceptr(addr);
    traceint(unknown);
    traceint(size);
    tracebuf(addr, size);
    result = callbacks->Free(addr, unknown, size);
    traceint(result);
    return result;
}

static void * __pascal _wrap_MapMemVmr(void far *ptr, int v)
{
    void *result;
    trace("callbacks->MapMem");
    traceptr(ptr);
    traceint(v);
    result = callbacks->MapMemVmr(ptr, v);
    traceptr(result);
    return result;
}

static void far * __pascal _wrap_LoadVmr(int vmr)
{
    void *result = 0;
    trace("callbacks->LoadVmr");
    traceint(vmr);
    result = callbacks->LoadVmr(vmr);
    traceptr(result);
    return result;
}

static void __pascal _wrap_UnregisterDevdata(int hdl)
{
    trace("callbacks->UnregisterDevdata");
    traceint(hdl);
    callbacks->UnregisterDevdata(hdl);
    return;
}

static unsigned __pascal _wrap_GetDescriptor(unsigned far *ptr, int base, int limit)
{
    int result;
    traceptr(ptr);
    traceval("%#x", *ptr);
    traceint(base);
    traceint(limit);
    trace("callbacks->GetDescriptor");
    result = callbacks->GetDescriptor(ptr, base, limit);
    traceint(*ptr);
    traceint(result);
    return result;
}

static void __pascal _wrap_FreeDescriptor(int seg)
{
    trace("callbacks->FreeDescriptor");
    traceint(seg);
    callbacks->FreeDescriptor(seg);
    return;
}

static void __pascal _wrap_exprt_scan_linx(int a, int b, int c, int d)
{
    trace("devfuncs->exprt_scan_linx");
    traceint(a);
    traceint(b);
    traceint(c);
    traceint(d);
    exprt_scan_linx(a, b, c, d);
    return;
}

static void __pascal _wrap_exprt_fill_rect(int a, int b, int c, int d, int e)
{
    trace("devfuncs->wrap_exprt_fill_rect");
    traceint(a);
    traceint(b);
    traceint(c);
    traceint(d);
    traceint(e);
    exprt_fill_rect(a, b, c, d, e);
    return;
}

static void __pascal _wrap_exprt_thin_diag_line(int a, int b, int c, int d, int e)
{
    trace("devfuncs->exprt_thin_diag_line");
    traceint(a);
    traceint(b);
    traceint(c);
    traceint(d);
    traceint(e);
    exprt_thin_diag_line(a, b, c, d, e);
    return;
}

static void __pascal _wrap_exprt_thin_vert_line(int a, int b, int c, int d)
{
    trace("devfuncs->exprt_thin_vert_line");
    traceint(a);
    traceint(b);
    traceint(c);
    traceint(d);
    exprt_thin_vert_line(a, b, c, d);
    return;
}

static void __pascal _wrap_exprt_shade_rect(int a, int b, int c, int d, void far *e, int f)
{
    trace("devfuncs->exprt_shade_rect");
    traceint(a);
    traceint(b);
    traceint(c);
    traceint(d);
    traceptr(e);
    traceint(f);
    exprt_shade_rect(a, b, c, d, e, f);
    return;
}

static void __pascal _wrap_exprt_fill_scan_list()
{
    trace("devfuncs->exprt_fill_scan_list");
    exprt_fill_scan_list();
    return;
}

static int __pascal _wrap_RegisterDevdataStruct(struct DEVDATA far *a, int far *b)
{
    int result;
    int i;
    trace("callbacks->RegisterDevdataStruct");
    traceptr(a);
    traceptr(b);
    tracebuf(a, sizeof *a);

    traceint(a->ShowMeFlag);
    traceint(a->RasterHeight);
    traceint(a->AspectX);
    traceint(a->AspectY);
    traceint(a->RHmusPerTHmu);
    traceint(a->RHmuPerGHmus);
    traceint(a->RVmusPerTVmu);
    traceint(a->RVmuPerGVmus);

    for (i = 0; i < 12; i++) {
        traceint(a->FillPatts[i].pattsize.pty);
        traceint(a->FillPatts[i].pattsize.ptx);
        traceint(a->FillPatts[i].patthotSpot.pty);
        traceint(a->FillPatts[i].patthotSpot.ptx);
        traceptr(a->FillPatts[i].pattptr);
        tracebuf(a->FillPatts[i].pattptr, 16);
        traceint(a->FillPatts[i].pattSecOffset.pty);
        traceint(a->FillPatts[i].pattSecOffset.ptx);
    }

    tracebuf(a->ColorMap, sizeof a->ColorMap);
    tracebuf(a->SrcPatD, sizeof a->SrcPatD);
    tracebuf(a->SrcPatS, sizeof a->SrcPatS);
    traceint(a->FIndexCnt);
    traceptr(a->FontIndex);
    traceint(a->FNamesCnt);
    traceptr(a->FontNames);

    // Install our wrappers.
    a->DevFuncs.fill_scan_list = _wrap_exprt_fill_scan_list;
    a->DevFuncs.shade_rect = _wrap_exprt_shade_rect;
    a->DevFuncs.thin_vert_line = _wrap_exprt_thin_vert_line;
    a->DevFuncs.thin_diag_line = _wrap_exprt_thin_diag_line;
    a->DevFuncs.fill_rect = _wrap_exprt_fill_rect;
    a->DevFuncs.scan_linx = _wrap_exprt_scan_linx;

    result = callbacks->RegisterDevdataStruct(a, b);
    traceint(result);
    traceint(*b);
    return result;
}

static int __pascal _wrap_field_64(int a, void far *ptr)
{
    int result;
    trace("callbacks->field_64");
    traceint(a);
    traceptr(ptr);
    tracebuf(ptr, 16);
    __breakpoint(0x297, 0x23d5);
    result = callbacks->field_64(a, ptr);
    __debugbreak();
    traceint(result);
    tracebuf(ptr, 16);
    return result;
}

static void __pascal _wrap_field_58(int a, int b)
{
    trace("callbacks->field_58");
    traceint(a);
    traceint(b);
    callbacks->field_58(a, b);
    return;
}

static int __pascal _wrap_field_5C(int a, void far *b)
{
    int result;
    trace("callbacks->field_5C");
    traceint(a);
    traceptr(b);
    result = callbacks->field_5C(a, b);
    traceint(result);
    return result;
}

static int __pascal _wrap_field_68(int a, int b, void far *ptr1, void far *ptr2)
{
    int result;

    trace("callbacks->field_68");
    traceint(a);
    traceint(b);
    traceptr(ptr1);
    traceptr(ptr2);
    result = callbacks->field_68(a, b, ptr1, ptr2);
    traceint(result);
    return result;
}

static int __pascal _wrap_field_6C(int a,
                                   int b,
                                   void *c,
                                   int d,
                                   void far *e,
                                   void far *f)
{
    int result;
    trace("callbacks->field_6C");
    traceint(a);
    traceint(b);
    traceptr(c);
    traceint(d);
    traceptr(e);
    traceptr(f);
    result = callbacks->field_6C(a, b, c, d, e, f);
    traceint(result);
    return result;
}

void InstallCallbackWrapper(struct LOTUSFUNCS *cbarray)
{
    memset(cbarray, 0xcc, sizeof *cbarray);
    cbarray->AllocMem = _wrap_AllocMem;
    cbarray->lfield_8 = _wrap_lfield_8;
    cbarray->Allocate = _wrap_Allocate;
    cbarray->Free     = _wrap_Free;
    cbarray->MapMemVmr= _wrap_MapMemVmr;
    cbarray->LoadVmr  = _wrap_LoadVmr;
    cbarray->RegisterDevdataStruct = _wrap_RegisterDevdataStruct;
    cbarray->field_58 = _wrap_field_58;
    cbarray->field_5C = _wrap_field_5C;
    cbarray->UnregisterDevdata = _wrap_UnregisterDevdata;
    cbarray->field_64 = _wrap_field_64;
    cbarray->field_68 = _wrap_field_68;
    cbarray->field_6C = _wrap_field_6C;
    cbarray->GetDescriptor = _wrap_GetDescriptor;
    cbarray->FreeDescriptor = _wrap_FreeDescriptor;
}

