# Lotus 1-2-3 R4D Display Driver for DOSEMU2

This is a <abbr title="Work in Progress">WIP</abbr> display driver for
Lotus 1-2-3 R4D to enable support for arbitrary text resolutions in
[DOSEMU2](https://github.com/dosemu2/dosemu2).

That means you can run 123 in a maximized xterm, and see more columns than
you could possibly want!

![screenshot](screenshot.png)

I used MSVC 8.00c (still available to MSDN subscribers), I believe it's the
last version of MSVC that could target DOS.

It almost works, but I haven't fully implemented all the API, so if you do
something unusual it might trap. It also has some debugging code that logs
all the calls, which makes it really slow.

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

I wrote the code on Linux, and then used dosemu to run the old toolchain.

For example, you can run tasm from a Makefile like `dosemu -dumb -E "tasm ..."`

The `-dumb` (i.e. dumb terminal) option makes old DOS tools work like UNIX
tools. This requires you have your autoexec.bat setup so that `%PATH%` works,
but is very convenient for development - even exit codes work!
