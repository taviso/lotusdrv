#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include "config.h"
#include "snprintf.h"

#include "caca.h"
#include "cacaint.h"

int caca_gotoxy(caca_canvas_t *cv, int x, int y)
{
    cv->frames[cv->frame].x = x;
    cv->frames[cv->frame].y = y;

    return 0;
}

int caca_wherex(caca_canvas_t const *cv)
{
    return cv->frames[cv->frame].x;
}

int caca_wherey(caca_canvas_t const *cv)
{
    return cv->frames[cv->frame].y;
}

int caca_put_char(caca_canvas_t *cv, int x, int y, uint32_t ch)
{
    cv->c[x + y * cv->width].c = caca_utf32_to_cp437(ch);
    cv->c[x + y * cv->width].attr = caca_attr_to_ansi(cv->curattr);
    return 1;
}

uint32_t caca_get_char(caca_canvas_t const *cv, int x, int y)
{
    if(x < 0 || x >= (int)cv->width || y < 0 || y >= (int)cv->height)
        return ' ';

    return caca_cp437_to_utf32(cv->c[x + y * cv->width].c);
}

int caca_put_str(caca_canvas_t *cv, int x, int y, char const *s)
{
    int len;

    if (y < 0 || y >= (int)cv->height || x >= (int)cv->width) {
        return strlen(s);
    }

    for (len = 0; *s; len++) {
        caca_put_char(cv, x + len, y, *s++);
    }

    return len;
}

int caca_printf(caca_canvas_t *cv, int x, int y, char const *format, ...)
{
    va_list args;
    int ret;
    va_start(args, format);
    ret = caca_vprintf(cv, x, y, format, args);
    va_end(args);
    return ret;
}

int caca_vprintf(caca_canvas_t *cv, int x, int y, char const *format,
                 va_list args)
{
    char tmp[256];
    char *buf = tmp;
    int bufsize = sizeof(tmp), ret;

    if(cv->width - x + 1 > sizeof(tmp))
    {
        return -1;
    }

    vsnprintf(buf, bufsize, format, args);
    buf[bufsize - 1] = '\0';

    ret = caca_put_str(cv, x, y, buf);

    return ret;
}

int caca_clear_canvas(caca_canvas_t *cv)
{
    uint32_t attr = cv->curattr;
    int n;

    for(n = cv->width * cv->height; n--; )
    {
        cv->c[n].c = ' ';
        cv->c[n].attr = caca_attr_to_ansi(attr);
    }

    return 0;
}

int caca_set_canvas_handle(caca_canvas_t *cv, int x, int y)
{
    cv->frames[cv->frame].handlex = x;
    cv->frames[cv->frame].handley = y;

    return 0;
}

int caca_get_canvas_handle_x(caca_canvas_t const *cv)
{
    return cv->frames[cv->frame].handlex;
}

int caca_get_canvas_handle_y(caca_canvas_t const *cv)
{
    return cv->frames[cv->frame].handley;
}

int caca_blit(caca_canvas_t *dst, int x, int y,
              caca_canvas_t const *src, caca_canvas_t const *mask)
{
    return -1;
}

int caca_set_canvas_boundaries(caca_canvas_t *cv, int x, int y, int w, int h)
{
    return -1;
}

