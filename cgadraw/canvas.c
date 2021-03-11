//
// This file is derived from libcaca, but modified to add simple drawing
// primitives to a CGA framebuffer for Lotus 1-2-3.
//
// The original copyright notice is below.

/*
 *  libcaca     Colour ASCII-Art library
 *  Copyright © 2002—2018 Sam Hocevar <sam@hocevar.net>
 *              All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include "config.h"
#include <stddef.h>
#include <dos.h>
#include <string.h>

#include "caca.h"
#include "cacaint.h"

caca_canvas_t * far caca_create_canvas(int width, int height, int sel, int off)
{
    static caca_canvas_t canvas;
    static struct caca_frame frame;
    caca_canvas_t * far cv;

    if (canvas.refcount != 0)
        return NULL;

    if (width < 0 || height < 0) {
        seterrno(EINVAL);
        return NULL;
    }

    cv = &canvas;
    cv->refcount++;
    cv->autoinc = 0;
    cv->resize_callback = NULL;
    cv->resize_data = NULL;

    cv->frame = 0;
    cv->framecount = 1;
    cv->frames = &frame;
    cv->frames[0].width = width;
    cv->frames[0].height = height;
    cv->frames[0].c = MK_FP(sel, off);
    cv->frames[0].x = cv->frames[0].y = 0;
    cv->frames[0].handlex = cv->frames[0].handley = 0;
    cv->frames[0].curattr = 0;
    cv->frames[0].name = "frame#00000000";

    _caca_load_frame_info(cv);
    caca_set_color_ansi(cv, CACA_DEFAULT, CACA_TRANSPARENT);

    cv->ndirty = 0;
    cv->dirty_disabled = 1;
    cv->ff = NULL;
    return cv;
}

int caca_manage_canvas(caca_canvas_t *cv, int (*callback)(void *), void *p)
{
    return -1;
}

int caca_unmanage_canvas(caca_canvas_t *cv, int (*callback)(void *), void *p)
{
    return -1;
}

int caca_set_canvas_size(caca_canvas_t *cv, int width, int height)
{
    return -1;
}

int caca_get_canvas_width(caca_canvas_t const *cv)
{
    return cv->width;
}

int caca_get_canvas_height(caca_canvas_t const *cv)
{
    return cv->height;
}

uint32_t const * caca_get_canvas_chars(caca_canvas_t const *cv)
{
    return NULL;
}

uint32_t const * caca_get_canvas_attrs(caca_canvas_t const *cv)
{
    return NULL;
}

int caca_free_canvas(caca_canvas_t *cv)
{
    cv->refcount--;
    return 0;
}

int caca_rand(int min, int max)
{
    return min;
}

int caca_resize(caca_canvas_t *cv, int width, int height)
{
    return -1;
}

