; vim: set ft=tasm:
version m520
.model large,stdcall
.486

DOSSEG
CODESEG

extern DriverInit: proc
extern GetDisplayInfo: proc
extern DriverTerminate: proc
extern ResetDisplay: proc
extern SetGraphicsMode: proc
extern MoveCursor: proc
extern WriteLmbcsStringWithAttributes: proc
extern WritePaddedLmbcsStringWithAttributes: proc
extern SetRegionBgAttributes: proc
extern ClearRegionForeground: proc
extern ClearRegionForegroundKeepBg: proc
extern BlockRegionCopy: proc
extern CalcSizeTranslatedString: proc
extern FitTranslatedString: proc
extern SetCursorInvisible: proc
extern SetCursorVisible: proc
extern LockCursorAttributes: proc
extern QQCalledBeforeGraphicsMode: proc
extern QQLotusApiJustCallsFunc_1: proc
extern MoveCursor2: proc
extern QQLotusApiJustCallsFunc_0: proc
extern ComplicatedNop: proc
extern zerosub_0: proc
extern LotusApiPassedPtr: proc
extern LotusApiJustCallsFunc: proc
extern nullsub_5: proc

start           dd GetDisplayInfo
                dd DriverInit
                dd DriverTerminate
                dd ResetDisplay
                dd SetGraphicsMode
                dd MoveCursor
                dd WriteLmbcsStringWithAttributes
                dd WritePaddedLmbcsStringWithAttributes
                dd SetRegionBgAttributes
                dd ClearRegionForeground
                dd ClearRegionForegroundKeepBg
                dd BlockRegionCopy
                dd CalcSizeTranslatedString
                dd FitTranslatedString
                dd SetCursorInvisible
                dd SetCursorVisible
                dd LockCursorAttributes
                dd QQCalledBeforeGraphicsMode
                dd QQLotusApiJustCallsFunc_1
                dd MoveCursor2
                dd QQLotusApiJustCallsFunc_0
                dd ComplicatedNop
                dd zerosub_0
                dd LotusApiPassedPtr
                dd LotusApiJustCallsFunc
                dd nullsub_5
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
