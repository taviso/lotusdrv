#ifndef __DEBUG_H
#define __DEBUG_H

// We have to do a lot of printf debugging, so a few macros to help.
#ifndef NDEBUG
# define trace(str) logmsg("%s:%d: TRACE %s", __FILE__, __LINE__, (str))
# define traceval(fmt, val)     \
    logmsg("%s:%d: TRACE %s " fmt, __FILE__, __LINE__, #val, (val))
# define traceint(val)          \
    logmsg("%s:%d: TRACE %s %d", __FILE__, __LINE__, #val, (val))
# define tracestr(val)          \
    logmsg("%s:%d: TRACE %s \"%s\"", __FILE__, __LINE__, #val, (val))
# define tracestrlen(val, len)  \
    logmsg("%s:%d: TRACE %s \"%.*s\"", __FILE__, __LINE__, #val, (len), (val))
# define traceptr(val)          \
    logmsg("%s:%d: TRACE %s %p", __FILE__, __LINE__, #val, (val))
# define tracechr(val)          \
    logmsg("%s:%d: TRACE %s %hd", __FILE__, __LINE__, #val, (val))
# define tracebuf(ptr, len) do {                                            \
    logmsg("%s:%d: TRACE %s (%d bytes):", __FILE__, __LINE__, #ptr, (len)); \
    loghex(ptr, len);                                                       \
} while (false)
# define traceent(name) do {                            \
    int ra, rs;                                         \
    __asm { mov ax, word ptr [bp+2] };                  \
    __asm { mov ra, ax              };                  \
    __asm { mov ax, word ptr [bp+4] };                  \
    __asm { mov rs, ax              };                  \
    logmsg("ENTER %s, caller %hx:%hx", (name), rs, ra); \
} while (false)
#else
# define trace(str)
# define traceval(fmt, val)
# define traceint(val)
# define tracestr(val)
# define traceptr(val)
# define tracechr(val)
# define traceent(name)
# define tracebuf(ptr, len)
# define tracestrlen(val, len)
#endif

// Put some useful values in registers so dumps are useful.
#define __debugbreak() do {     \
    int   line = __LINE__;      \
    char *file = __FILE__;      \
    __asm { mov si, line };     \
    __asm { les di, file };     \
    __asm { int 3        };     \
} while (false)

// I cant use conditional breakpoints, this will have to do.
#define __breakafter(n) do {    \
    static int count;           \
    if (++count >= (n))         \
        __asm { int 3 };        \
} while (false)

// These get replaced on overlay switch, but sometimes handy.
#define __breakpoint(s, o) do {             \
    unsigned char far *bpint = MK_FP(s, o); \
    *bpint = 0xCC;                          \
} while (false)

int openlog(const char *name);
int closelog(void);
int logmsg(const char *fmt, ...);
int logstr(const char *fmt, ...);
void loghex(const void far *ptr, int buflen);

#endif
