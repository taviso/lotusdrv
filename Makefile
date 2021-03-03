DFLAGS=-quiet -dumb
WARNLEVEL=4
ASFLAGS=/W$(WARNLEVEL) /WX /nologo
#CPPFLAGS=/DNDEBUG
CPPFLAGS=
CFLAGS=/AL /nologo /NDAAA /Od /G3 /Gs /Gc /Zi /FPi87 /Zp1 /Zl /Gf /f- /W$(WARNLEVEL) /WX
LDFLAGS=/b /nologo /onerror:noexe /nod /noe
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
	dosemu $(DFLAGS) -E "IN G:/ ML /c $(ASFLAGS) /Fo$@ $<"

%.exe: %.obj
	printf "$^,$@,$(@:.exe=),$(LDLIBS),nul\r\n" > $(@:.exe=.lnk)
	dosemu $(DFLAGS) -E "IN G:/ LINK $(LDFLAGS) @$(@:.exe=.lnk)"
	@rm -f $(@:.exe=.lnk)

%.dld: %.exe
	mv $< $@
#	dosemu $(DFLAGS) -E "IN G:/ EXEHDR $(EFLAGS) $@"

all: l13vdemu.dld

# Third party code, reduce warning level
snprintf.obj: WARNLEVEL=1

l13vdemu.exe: l13vdemu.obj lotdemu.obj snprintf.obj debug.obj bundle.obj \
              lmbcs.obj attr.obj

clean:
	rm -f *.exe *.obj *.dld *.map *.pdb *.lnk *.cl
	chmod u=rw,go=r *.asm *.names
