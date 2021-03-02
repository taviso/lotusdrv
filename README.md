# Lotus 1-2-3 R4D Display Driver for DOSEMU2

This is a <abbr title="Work in Progress">WIP</abbr> display driver for
Lotus 1-2-3 R4D to enable support for arbitrary text resolutions in
[DOSEMU2](https://github.com/dosemu2/dosemu2).

That means you can run 123 in a maximized xterm, and see more columns than
you could possibly want!

![screenshot](screenshot.png)

> Note: This driver is intended for DOSEMU2 in *-term* mode, i.e. running in a terminal.

I used MSVC 8.00c (still available to MSDN subscribers), I believe it's the
last version of MSVC that could target DOS.

In case you're wondering, DJGPP (the GCC port to DOS) can only produce 32-bit
[DPMI](https://en.wikipedia.org/wiki/DOS_Protected_Mode_Interface) code, but
123 is a true 16-bit real mode application. It *might* be possible to switch in
and out of dpmi mode with a small stub, but 123 uses a lot of memory management
tricks and frankly intermixing the two seems really hard. I think it's a better
solution to stay in real mode.

This driver almost works, but I haven't fully implemented all the API, so if you do
something unusual it might trap. It might take a few more weekends to be totally
stable! At the moment, it also has some debugging code that logs all the calls,
I'll remove that when it's ready for other people to use.

I highly doubt anybody else will want to use this, but if you are excited
to help hacking on this, or writing modern add-ins for 1-2-3, I'd love to hear
about it!

Future ideas:

- A FILE driver that adds native support for XLS, or ODT?
- libcaca support so that text mode graphs work?
- More modern @functions?

I have some development notes available
[here](https://lock.cmpxchg8b.com/lotus123.html) and
[here](https://lock.cmpxchg8b.com/lotusinternals.html)

# Building

> If you just want a binary to try, check out the Releases section.

Note that this code isn't really ready for non-developer use yet, but you're
welcome to try it out and tell me about any visual glitches or crashes!

I wrote the code on Linux, and then used dosemu to run the old toolchain.

For example, you can run tasm from a Makefile like `dosemu -dumb -E "tasm ..."`

The `-dumb` (i.e. dumb terminal) option makes old DOS tools work like UNIX
tools. This requires you have your autoexec.bat setup so that `%PATH%` works,
but is very convenient for development - even exit codes work!

