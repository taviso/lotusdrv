#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <conio.h>
#include <dos.h>

#include "config.h"
#include "debug.h"
#include "idatypes.h"
#include "lottypes.h"
#include "lotcalls.h"
#include "bundle.h"
#include "lmbcs.h"
#include "attr.h"

// These map 1-2-3's character attribute flags onto CGA character attributes.
// i.e.
//
// NOTE: I tend to write 123 attributes in octal, and CGA attributes in hex.
//
//                               BG  FG
//   123  CGA    Decoded         *RGB*RGB   BG        FG (* is bright or blinking)
//   000  0x07   0b00000111 -> 0b00000111   WHITE  /  BLACK
//   001  0x30   0b00110000 -> 0b00110000   CYAN   /  WHITE
//   002  0x17   0b00010111 -> 0b00010111   BLUE   /  BLACK
//   003  0xC0   0b11000000 -> 0b11000000  *RED    /  WHITE
//   030  0x06   0b00000110 -> 0b00000110  BLACK   /  YELLOW
//   ...and so on.
//
//  123 has 6 bits of attributes, 3 FG and 3 BG bits
//
//  111 111
//
//  CGA has two additional bits, for bright/blinking.
//
const unsigned char lot_char_attr_table[0100] = {
   0x07, 0x30, 0x17, 0xC0, 0x70, 0x30, 0x10, 0x40,
   0x01, 0x32, 0x13, 0xC1, 0x71, 0x31, 0x11, 0x41,
   0x02, 0x31, 0x12, 0xC2, 0x72, 0x32, 0x12, 0x42,
   0x06, 0x36, 0x16, 0xC6, 0x73, 0x33, 0x13, 0x43,
   0x04, 0x34, 0x14, 0xC7, 0x74, 0x34, 0x14, 0x44,
   0x09, 0x3A, 0x1B, 0xC9, 0x75, 0x35, 0x15, 0x45,
   0x0C, 0x3C, 0x1C, 0xCF, 0x76, 0x36, 0x16, 0x46,
   0x0E, 0x3E, 0x1E, 0xCE, 0x77, 0x37, 0x17, 0x47,
};

