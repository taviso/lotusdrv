DFLAGS=-quiet -dumb
ASFLAGS=/m3 /q /z /w0
#CPPFLAGS=/DNDEBUG
CPPFLAGS=
WARNLEVEL=4
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
	dosemu $(DFLAGS) -E "IN G:/ TASM32 /c $(ASFLAGS) $<"

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

# Original driver.
l13vcgaf.exe: l13vcgaf.obj

# Original driver patched to add debugging calls.
l13vcgad.exe: l13vcgad.obj logcalls.obj logcbs.obj snprintf.obj debug.obj \
              l13vcgaf.obj

release: CPPFLAGS+=/DRELEASE
release: clean l13vdemu.dld
	zip l13vdemu.zip l13vdemu.dld README.md


clean:
	rm -f *.exe *.obj *.dld *.map *.pdb *.lnk *.cl l13vdemu.zip
	chmod u=rw,go=r *.asm *.names
