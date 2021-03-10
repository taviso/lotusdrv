DFLAGS=-quiet -dumb
ASFLAGS=/m3 /q /z /w0
CPPFLAGS=/Iinclude
WARNLEVEL=4
CFLAGS=/AL /nologo /NDAAA /Od /G3 /Gs /Gc /Zi /FPi87 /Zp1 /Zl /Gf /f- /W$(WARNLEVEL) /WX
LDFLAGS=/b /nologo /onerror:noexe /nod /noe
LDLIBS=LLIBC7
EFLAGS=/verbose /nologo

.PHONY: clean

# MS-DOS doesn't allow long commandlines, so params are written to a "response"
# file.

%.obj: %.c
	@rm -f $@
	printf "$(CPPFLAGS) $(CFLAGS)\r\n" > $(@:.obj=.cl)
	dosemu $(DFLAGS) -E "IN G:/ CL /c @$(@:.obj=.cl) /Fo$@ $<"
	@rm -f $(@:.obj=.cl)

%.obj: %.asm
	@rm -f $@
	dosemu $(DFLAGS) -E "IN G:/ TASM32 /c $(ASFLAGS) $<"

all: l13vcgad.obj l13vcgaf.obj logcalls.obj logcbs.obj

clean:
	rm -f *.exe *.obj *.dld *.map *.pdb *.lnk *.cl
	chmod u=rw,go=r *.asm *.names *.idb
