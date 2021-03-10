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

// This is a debugging shim that logs what a real driver is doing.
//
// The idea is that I can compare the logs with what my driver is doing, and
// check where they disagree.
//
// Because 123 uses callee clears calling convention, unfortunately I
// have to know the correct parameters, and can't just use a generic
// logger.

struct LOTUSFUNCS far *callbacks;

struct LOTUSFUNCS cbwrappers;

extern void InstallCallbackWrapper(struct LOTUSFUNCS *cbarray);

extern int __pascal GetDisplayInfo(struct DISPLAYINFO far *dspinfo);

int __pascal _wrap_GetDisplayInfo(struct DISPLAYINFO far *dspinfo)
{
    int result;
    traceent("GetDisplayInfo");
    traceptr(dspinfo);
    result = GetDisplayInfo(dspinfo);
    tracebuf(dspinfo, sizeof *dspinfo);
    return result;
}

extern int __pascal DriverInit(void *drvapi,
                               void *vbdptr,
                               char *cfgname,
                               char deflmbcsgrp);
static unsigned d;

int __pascal _wrap_DriverInit(void *drvapi,
                              void *vbdptr,
                              char *cfgname,
                              char deflmbcsgrp)
{
    int result;
    openlog("H:\\CORRECT.LOG");
    traceent("DriverInit");

    traceptr(drvapi);
    traceptr(vbdptr);
    tracestr(cfgname);
    tracechr(deflmbcsgrp);

    // Save a pointer to real callbacks.
    callbacks = drvapi;

    // Make a copy of these callbacks so that I can patch them and replace
    // with debugging wrappers.
    memcpy(&cbwrappers, callbacks, sizeof *callbacks);

    tracebuf(callbacks, sizeof *callbacks);

    InstallCallbackWrapper(&cbwrappers);

    result = DriverInit(&cbwrappers, vbdptr, cfgname, deflmbcsgrp);
    traceint(result);

    trace("XXX");
    callbacks->GetDescriptor(&d, 0, 0x100);
    traceint(d);
    return result;
}

extern int __pascal DriverTerminate();
int __pascal _wrap_DriverTerminate()
{
    int result;
    traceent("DriverTerminate");
    //closelog();
    result = DriverTerminate();
    trace("XXX");
    callbacks->FreeDescriptor(d);
    traceint(result);
    return result;
}

extern int __pascal ResetDisplay();
int __pascal _wrap_ResetDisplay()
{
    traceent("ResetDisplay");
    return ResetDisplay();
}

extern int __pascal SetGraphicsMode();
int __pascal _wrap_SetGraphicsMode()
{
    traceent("SetGraphicsMode");
    return SetGraphicsMode();
}

void __pascal _wrap_MoveCursor(int col, int line)
{
    traceent("MoveCursor");
    traceint(col);
    traceint(line);
    MoveCursor(col, line);
    return;
}

extern int __pascal WriteLmbcsStringWithAttributes(int byteslen,
                                                   char far *lmbcsptr,
                                                   char attrs);
int __pascal _wrap_WriteLmbcsStringWithAttributes(int byteslen,
                                                  char far *lmbcsptr,
                                                  char attrs)
{
    int result;
    traceent("WriteLmbcsStringWithAttributes");
    traceint(byteslen);
    tracestrlen(lmbcsptr, byteslen);
    traceval("%x", attrs);
    result = WriteLmbcsStringWithAttributes(byteslen, lmbcsptr, attrs);
    traceint(result);
    return result;
}

extern int __pascal WritePaddedLmbcsStringWithAttributes(int byteslen,
                                                         char far *lmbcsptr,
                                                         char attrs,
                                                         int startpad,
                                                         int endpad);
int __pascal _wrap_WritePaddedLmbcsStringWithAttributes(int byteslen,
                                                        char far *lmbcsptr,
                                                        char attrs,
                                                        int startpad,
                                                        int endpad)
{
    int result;
    traceent("WritePaddedLmbcsStringWithAttributes");
    traceint(byteslen);
    tracestrlen(lmbcsptr, byteslen);
    traceval("%x", attrs);
    traceint(startpad);
    traceint(endpad);
    result = WritePaddedLmbcsStringWithAttributes(byteslen, lmbcsptr, attrs, startpad, endpad);
    traceint(result);
    return result;
}

extern int __pascal SetRegionBgAttributes(int cols, int lines, char attrs);
int __pascal _wrap_SetRegionBgAttributes(int cols, int lines, char attrs)
{
    traceent("SetRegionBgAttributes");
    traceint(cols);
    traceint(lines);
    traceval("%x", attrs);
    return SetRegionBgAttributes(cols, lines, attrs);
}

void __pascal _wrap_ClearRegionForeground(int cols, int lines, char attrs)
{
    traceent("ClearRegionForeground");
    traceint(cols);
    traceint(lines);
    traceval("%x", attrs);
    ClearRegionForeground(cols, lines, attrs);
    return;
}

extern int __pascal ClearRegionForegroundKeepBg(int cols, int lines);
int __pascal _wrap_ClearRegionForegroundKeepBg(int cols, int lines)
{
    traceent("ClearRegionForegroundKeepBg");
    traceint(cols);
    traceint(lines);
    return ClearRegionForegroundKeepBg(cols, lines);
}

extern int __pascal BlockRegionCopy();
int __pascal _wrap_BlockRegionCopy()
{
    traceent("BlockRegionCopy");
    __debugbreak();
    return 0;
}

extern int __pascal CalcSizeTranslatedString(int argstrlen, char far *argstr);
int __pascal _wrap_CalcSizeTranslatedString(int argstrlen, char far *argstr)
{
    int result;
    traceent("CalcSizeTranslatedString");
    tracestrlen(argstr, argstrlen);
    traceint(argstr);
    result = CalcSizeTranslatedString(argstrlen, argstr);
    traceint(result);
    return result;
}

extern int __pascal FitTranslatedString(int strarglen,
                                        char far *strarg,
                                        int ncols,
                                        int far *bytesneeded);
int __pascal _wrap_FitTranslatedString(int strarglen,
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

    result = FitTranslatedString(strarglen, strarg, ncols, bytesneeded);

    traceint(result);
    traceint(*bytesneeded);
    return result;
}

extern int __pascal SetCursorInvisible();
int __pascal _wrap_SetCursorInvisible()
{
    traceent("SetCursorInvisible");
    return SetCursorInvisible();
}

extern int __pascal SetCursorVisible();
int __pascal _wrap_SetCursorVisible()
{
    traceent("SetCursorVisible");
    return SetCursorVisible();
}

extern int __pascal LockCursorAttributes();
int __pascal _wrap_LockCursorAttributes()
{
    traceent("LockCursorAttributes");
    return LockCursorAttributes();
}

extern int __pascal QQCalledBeforeGraphicsMode(void far *p);
int __pascal _wrap_QQCalledBeforeGraphicsMode(void far *p)
{
    int result;
    traceent("QQCalledBeforeGraphicsMode");
    traceptr(p);
    result = QQCalledBeforeGraphicsMode(p);
    return result;
}

extern int __pascal QQLotusApiJustCallsFunc_1(int strarglen, void far *strarg);
int __pascal _wrap_QQLotusApiJustCallsFunc_1(int strarglen, void far *strarg)
{
    int result;
    traceent("QQLotusApiJustCallsFunc_1");
    traceint(strarglen);
    tracebuf(strarg, strarglen);
    result = QQLotusApiJustCallsFunc_1(strarglen, strarg);
    traceint(result);
    return result;
}

extern int __pascal MoveCursor2(int col, int line);
int __pascal _wrap_MoveCursor2(int col, int line)
{
    int result;
    traceent("MoveCursor2");
    traceint(col);
    traceint(line);
    result = MoveCursor2(col, line);
    traceint(result);
    return result;
}

extern int __pascal QQLotusApiJustCallsFunc_0(void far *ptr);
int __pascal _wrap_QQLotusApiJustCallsFunc_0(void far *ptr)
{
    int result;
    traceent("QQLotusApiJustCallsFunc_0");
    traceptr(ptr);
    tracebuf(ptr, 16);
    result = QQLotusApiJustCallsFunc_0(ptr);
    traceint(result);
    tracebuf(ptr, 16);
    return result;
}

extern int __pascal ComplicatedNop();
int __pascal _wrap_ComplicatedNop()
{
    traceent("ComplicatedNop");
    __debugbreak();
    return 0;
}

extern int __pascal zerosub_0();
int __pascal _wrap_zerosub_0()
{
    traceent("zerosub_0");
    __debugbreak();
    return 0;
}

extern int __pascal LotusApiPassedPtr(void far *ptr);
int __pascal _wrap_LotusApiPassedPtr(void far *ptr)
{
    traceent("LotusApiPassedPtr");
    traceptr(ptr);
    return LotusApiPassedPtr(ptr);
}

extern int __pascal LotusApiJustCallsFunc(int a, void far *ptra, int d, void far *ptrb);
int __pascal _wrap_LotusApiJustCallsFunc(int a, void far *ptra, int d, void far *ptrb)
{
    int result;
    traceent("LotusApiJustCallsFunc");
    traceint(a);
    traceptr(ptra);
    tracebuf(ptra, 16);
    traceint(d);
    traceptr(ptrb);
    tracebuf(ptrb, 16);
    result = LotusApiJustCallsFunc(a, ptra, d, ptrb);
    traceint(result);
    return result;
}

extern int __pascal nullsub_5();
int __pascal _wrap_nullsub_5()
{
    traceent("nullsub_5");
    __debugbreak();
    return 0;
}

