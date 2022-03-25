#ifndef BNKERNEL
#define BNKERNEL

#include <stddef.h>
#include <stdlib.h>
// #include <linux/slab.h>
// #include <sys/types.h>

#include <limits.h>
#include <string.h>
struct bn {
    unsigned int *number;
    size_t size;
};

struct bn *bn_new(size_t size);
void bn_free(struct bn *src);
void bn_resize(struct bn *src, size_t size);
void bn_multbytwo(struct bn *src);
struct bn *bn_square(struct bn *src);
struct bn *bn_mult(struct bn *a, struct bn *b);
char *bn_to_string(const struct bn *src);
struct bn *bn_add(struct bn *a, struct bn *b);
struct bn *bn_sub(struct bn *a, struct bn *b);
#endif