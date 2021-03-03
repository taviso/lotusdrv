#ifndef __CONFIG_H
#define __CONFIG_H

/* Warnings I don't care about */
#pragma warning(disable : 4001) // single-line comments
#pragma warning(disable : 4096) // varargs are always cdecl
#pragma warning(disable : 4127) // conditional expression constant
#pragma warning(disable : 4704) // in-line assembler
#pragma warning(disable : 4505) // unreferenced local function

// Warnings i don't mind unless i'm debugging.
#ifdef NDEBUG
# pragma warning(disable : 4100) // unreferenced formal parameter
# pragma warning(disable : 4101) // unreferenced local variable
#endif


// Unfortunately we can't use stdio in addins, because the crt uses malloc.
#define HAVE_SNPRINTF
#define PREFER_PORTABLE_SNPRINTF
#include "snprintf.h"

#define snprintf portable_snprintf
#define vsnprintf portable_vsnprintf

#define true 1
#define false 0

// In text mode, the screen resolution is queried via int 10,f:
// https://stanislavs.org/helppc/int_10-f.html
//
// The number of columns is returned in AL, so it cannot possibly
// exceed 256, because that's only an 8bit register.
#define MAX_COLS 256

// The selectors for the BIOS Data Area and CGA Framebuffer. We
// can't access these directly, we need to request 123 installs
// an LDT entry for us.
#define BIOS_DATA_AREA 0x0040
#define CGA_FRAMEBUF 0xb800

#endif
