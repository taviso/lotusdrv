DFLAGS=-quiet -dumb
WARNLEVEL=0
CPPFLAGS=/I.
CFLAGS=/AL /nologo /NDAAB /Od /G3 /Gs /Gc /Zi /FPi87 /Zp1 /Zl /Gf /f- /W$(WARNLEVEL) /WX
LDFLAGS=
LDLIBS=CGADRAW

.PHONY: clean

# MS-DOS doesn't allow long commandlines, so params are written to a "response"
# file.

%.obj: %.c
	@rm -f $@
	printf "$(CPPFLAGS) $(CFLAGS)\r\n" > $(@:.obj=.cl)
	dosemu $(DFLAGS) -E "IN G:/ CL /c @$(@:.obj=.cl) /Fo$@ $<"
	@rm -f $(@:.obj=.cl)

%.lib: %.obj
	@rm -f $@
	printf "$(LFLAGS) $(addprefix +,$^),nul\r\n" > $(@:.lib=.cl)
	dosemu $(DFLAGS) -E "IN G:/ LIB $@ @$(@:.lib=.cl)"
	@rm -f $(@:.lib=.cl)

%.exe: %.obj
	@rm -f $@
	printf "$(CFLAGS) $(LDFLAGS) $^ $(addprefix /link ,$(LDLIBS))\r\n" > $(@:.exe=.cl)
	dosemu $(DFLAGS) -E "IN G:/ CL /Fe$@ @$(@:.exe=.cl)"
	@rm -f $(@:.exe=.cl)

all: cgadraw.lib

drawtest.obj: CFLAGS:=$(filter-out /Zl,$(CFLAGS))
drawtest.exe: snprintf.obj | cgadraw.lib

cgadraw.lib: attr.obj box.obj canvas.obj charset.obj conic.obj frame.obj \
             line.obj string.obj transfrm.obj triangle.obj

clean:
	rm -f *.exe *.lib *.obj *.map *.pdb *.cl
