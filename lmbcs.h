#ifndef __LMBCS_H
#define __LMBCS_H

int translate_lmbcs(const void *src,
                    unsigned char *dst,
                    int maxdst,
                    int maxsrc,
                    int defgroup);

#endif
