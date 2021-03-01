DFLAGS=-quiet -dumb
ASFLAGS=/m3 /q /z /w0
#CPPFLAGS=/DNDEBUG
CPPFLAGS=
WARNLEVEL=4
CFLAGS=/AL /nologo /NDAAA /O1i /G3 /Gs /Gc /Zi /FPi87 /Zp1 /Zl /Gf /f- /W$(WARNLEVEL) /WX
LDFLAGS=/b /map:full /nologo /info /onerror:noexe /nod /noe /packd /packc
LDLIBS=LLIBC7
EFLAGS=/verbose /nologo

.PHONY: clean

# MS-DOS doesn't allow long commandlines, so params are written to a "response"
# file.

%.obj: %.c
	printf "$(CPPFLAGS) $(CFLAGS)\r\n" > $(@:.obj=.cl)
	dosemu $(DFLAGS) -E "IN G:/ CL /c @$(@:.obj=.cl) /Fo$@ $<"
	@rm -f $(@:.obj=.cl)

%.obj: %.asm
	dosemu $(DFLAGS) -E "IN G:/ TASM32 $(ASFLAGS) $<"

%.exe: %.obj
	printf "$^,$@,$(@:.exe=),$(LDLIBS),nul\r\n" > $(@:.exe=.lnk)
	dosemu $(DFLAGS) -E "IN G:/ LINK $(LDFLAGS) @$(@:.exe=.lnk)"
	@rm -f $(@:.exe=.lnk)

%.dld: %.exe
	mv $< $@
#	dosemu $(DFLAGS) -E "IN G:/ EXEHDR $(EFLAGS) $@"

all: l13vdemu.dld l13vcgaf.dld l13vcgad.dld

# Third party code, reduce warning level
snprintf.obj: WARNLEVEL=1

l13vcgaf.exe: l13vcgaf.obj
l13vdemu.exe: l13vdemu.obj lotdemu.obj snprintf.obj debug.obj bundle.obj \
              lmbcs.obj attr.obj
l13vcgad.exe: l13vcgad.obj logcalls.obj logcbs.obj snprintf.obj debug.obj \
              l13vcgaf.obj

clean:
	rm -f *.exe *.obj *.dld *.map *.pdb *.lnk *.cl
	chmod u=rw,go=r *.asm *.names
