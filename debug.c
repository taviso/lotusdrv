#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <conio.h>
#include <dos.h>

#include "config.h"
#include "debug.h"

static int logfile = -1;

int openlog(const char *name)
{
    closelog();

    if (name == NULL)
        return 1;

    if (_dos_creat(name, _A_NORMAL, &logfile) != 0) {
        logmsg("_dos_creat failed to open logfile %s", name);
        return 1;
    }

    return 0;
}

int closelog(void)
{

    // If we're not logging, just return.
    if (logfile == -1) {
        return 1;
    }

    _dos_close(logfile);

    logfile = -1;

    return 0;
}

int logstr(const char *fmt, ...)
{
    va_list ap;
    unsigned len;
    char line[128];

    // If we're not logging, just return.
    if (logfile == -1)
        return 1;

    va_start(ap, fmt);

    portable_vsnprintf(line, sizeof(line), fmt, ap);

    va_end(ap);

    _dos_write(logfile, line, strlen(line), &len);

    return 0;
}


int logmsg(const char *fmt, ...)
{
    va_list ap;
    unsigned len;
    char line[128];

    // If we're not logging, just return.
    if (logfile == -1)
        return 1;

    va_start(ap, fmt);

    portable_vsnprintf(line, sizeof(line), fmt, ap);

    va_end(ap);

    _dos_write(logfile, line, strlen(line), &len);
    _dos_write(logfile, "\r\n", 2, &len);

    return 0;
}

void loghex(const void far *ptr, int buflen) {
    const unsigned char *buf = ptr;
    int i, j;

    // If we're not logging, just return.
    if (logfile == -1)
        return;

    for (i = 0; i < buflen; i += 16) {
        logstr("%06x: ", i);
        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                logstr("%02x ", buf[i+j]);
            else
                logstr("   ");
        logstr(" ");

        for (j = 0; j < 16; j++) {
            if (i + j < buflen) {
                if (buf[i+j] >= ' ' && buf[i+j] <= '~') {
                    logstr("%c", buf[i+j]);
                } else {
                    logstr(".");
                }
            }
        }

        logstr("\r\n");
    }
}

