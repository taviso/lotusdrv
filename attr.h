#ifndef __ATTR_H
#define __ATTR_H

// The octal usage here is intentional, doesn't it make the numbers look nice?

extern const unsigned char lot_char_attr_table[0100];

// Masks to change only bg/fg.
#define ATTR_FG   070
#define ATTR_BG   007
#define ATTR_ALL  077
#define ATTR_NONE 000

// Sometimes lotus will pass a BG attribute only.
#define MKFG(c) (unsigned char)((((c) & 007) << 3))

// Just for symmetry, not sure it's needed.
#define MKBG(c) (unsigned char)((((c) & 007)))

// If you need to specify them both.
#define MKFGBG(f,b) (unsigned char)(((((b) & 007) << 3)) | ((((f) & 007))))

// Translate a 123 attribute into a CGA attribute.
#define LATTR(c) (lot_char_attr_table[(c) & ATTR_ALL])

#endif
