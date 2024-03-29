DFLAGS=-quiet -dumb
ASFLAGS=/m3 /q /z /w0
#CPPFLAGS=/Icgadraw /I. /DNDEBUG
CPPFLAGS=/Icgadraw /I.
WARNLEVEL=4
CFLAGS=/AL /nologo /NDAAA /Od /G3 /Gs /Gc /Zi /FPi87 /Zp1 /Zl /Gf /f- /W$(WARNLEVEL)
LDFLAGS=/map:full /b /nologo /onerror:noexe /nod /noe
LDLIBS=LLIBC7
EFLAGS=/verbose /nologo
LFLAGS=-q

.PHONY: clean distclean cgadraw.lib l13vcgaf.obj l13vcgad.obj logcalls.obj logcbs.obj

%.plc: %.pl
	@rm -f $@
	dosemu $(DFLAGS) -E "IN G:/ LPL $(LFLAGS) $<"

%.plh: %.plc
	@rm -f $@
	dosemu $(DFLAGS) -E "IN G:/ DESCRIBE $<"

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

%.exe: %.obj
	@rm -f $@
	printf "$^,$@,$(@:.exe=),$(LDLIBS),nul\r\n" > $(@:.exe=.lnk)
	dosemu $(DFLAGS) -E "IN G:/ LINK $(LDFLAGS) @$(@:.exe=.lnk)"
	@rm -f $(@:.exe=.lnk)

%.dld: %.exe
	mv $< $@
#	dosemu $(DFLAGS) -E "IN G:/ EXEHDR $(EFLAGS) $@"

all: l13vdemu.dld

cgadraw.lib:
	make -C cgadraw $@
	@cp cgadraw/$@ $@

l13vcgaf.obj l13vcgad.obj logcalls.obj logcbs.obj:
	make -C debug $@
	@cp debug/$@ $@

# Third party code, reduce warning level
snprintf.obj: WARNLEVEL=1

l13vdemu.exe: LDLIBS+=CGADRAW
l13vdemu.exe: l13vdemu.obj lotdemu.obj snprintf.obj debug.obj bundle.obj \
              lmbcs.obj attr.obj draw.obj raster.obj | cgadraw.lib

# Original driver.
l13vcgaf.exe: l13vcgaf.obj

# Original driver patched to add debugging calls.
l13vcgad.exe: l13vcgad.obj logcalls.obj logcbs.obj snprintf.obj debug.obj \
              l13vcgaf.obj

release: CPPFLAGS+=/DRELEASE
release: clean l13vdemu.dld
	zip l13vdemu.zip l13vdemu.dld README.md

install: l13vdemu.dld
	cp $< ~/.dosemu/drive_c/123R4D/L13VCGAF.DLD

clean:
	rm -f *.exe *.obj *.dld *.map *.pdb *.lnk *.cl *.lib *.plc l13vdemu.zip

distclean: clean
	make -C cgadraw clean
	make -C debug clean
	make -C addin clean
