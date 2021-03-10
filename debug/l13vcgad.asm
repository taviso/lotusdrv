; vim: set ft=tasm:

                version m520
                .model large,stdcall
                .486

DOSSEG
CODESEG

extern  _wrap_DriverInit: proc
extern  _wrap_GetDisplayInfo: proc
extern  _wrap_DriverTerminate: proc
extern  _wrap_ResetDisplay: proc
extern  _wrap_SetGraphicsMode: proc
extern  _wrap_MoveCursor: proc
extern  _wrap_WriteLmbcsStringWithAttributes: proc
extern  _wrap_WritePaddedLmbcsStringWithAttributes: proc
extern  _wrap_SetRegionBgAttributes: proc
extern  _wrap_ClearRegionForeground: proc
extern  _wrap_ClearRegionForegroundKeepBg: proc
extern  _wrap_BlockRegionCopy: proc
extern  _wrap_CalcSizeTranslatedString: proc
extern  _wrap_FitTranslatedString: proc
extern  _wrap_SetCursorInvisible: proc
extern  _wrap_SetCursorVisible: proc
extern  _wrap_LockCursorAttributes: proc
extern  _wrap_QQCalledBeforeGraphicsMode: proc
extern  _wrap_QQLotusApiJustCallsFunc_1: proc
extern  _wrap_MoveCursor2: proc
extern  _wrap_QQLotusApiJustCallsFunc_0: proc
extern  _wrap_ComplicatedNop: proc
extern  _wrap_zerosub_0: proc
extern  _wrap_LotusApiPassedPtr: proc
extern  _wrap_LotusApiJustCallsFunc: proc
extern  _wrap_nullsub_5: proc

start           dd _wrap_GetDisplayInfo
                dd _wrap_DriverInit
                dd _wrap_DriverTerminate
                dd _wrap_ResetDisplay
                dd _wrap_SetGraphicsMode
                dd _wrap_MoveCursor
                dd _wrap_WriteLmbcsStringWithAttributes
                dd _wrap_WritePaddedLmbcsStringWithAttributes
                dd _wrap_SetRegionBgAttributes
                dd _wrap_ClearRegionForeground
                dd _wrap_ClearRegionForegroundKeepBg
                dd _wrap_BlockRegionCopy
                dd _wrap_CalcSizeTranslatedString
                dd _wrap_FitTranslatedString
                dd _wrap_SetCursorInvisible
                dd _wrap_SetCursorVisible
                dd _wrap_LockCursorAttributes
                dd _wrap_QQCalledBeforeGraphicsMode
                dd _wrap_QQLotusApiJustCallsFunc_1
                dd _wrap_MoveCursor2
                dd _wrap_QQLotusApiJustCallsFunc_0
                dd _wrap_ComplicatedNop
                dd _wrap_zerosub_0
                dd _wrap_LotusApiPassedPtr
                dd _wrap_LotusApiJustCallsFunc
                dd _wrap_nullsub_5
                dw seg _DATA

; This is just here to satisfy the crt, it should never be called.
global _main: proc
_main proc far
        mov     ax, 4141h
        int     3
_main endp

; There is an object file that screws up my alignment in the CRT, and
; I don't even need it.
;
; If you define a symbol, and then use /NOE, the linkers uses this one,
; so this stops that nuisance object from being linked into my executable.
global __nullcheck: proc
__nullcheck proc far
        mov     ax, 4242h
        int     3
__nullcheck endp

end start
