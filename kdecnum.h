#ifndef KDECNUM_H
#define KDECNUM_H

#include <linux/slab.h>
#include <linux/stddef.h>
#include <linux/types.h>

struct kdecnum {
    __u32 size;
    __u32 cap;
    int32_t *digits;
};

#define KDECNUM_INIT(a, b)                      \
    {                                           \
        .size = (a), .cap = (b), .digits = NULL \
    }

typedef struct kdecnum kdecnum_t;

#define DECMAXVALUE 1000000000  // 10^9

void kdecnum_new(kdecnum_t *ptr);
void kdecnum_free(kdecnum_t *ptr);
void kdecnum_swap(kdecnum_t *b1, kdecnum_t *b2);
void kdecnum_clean(kdecnum_t *b1);
void kdecnum_assign(kdecnum_t *b1, kdecnum_t *b2);
void kdecnum_add(kdecnum_t *b1, kdecnum_t *b2, kdecnum_t *result);
void kdecnum_sub(kdecnum_t *b1, kdecnum_t *b2, kdecnum_t *result);
void kdecnum_mult(const kdecnum_t *b1, const kdecnum_t *b2, kdecnum_t *result);
void kdecnum_multi_by_two(kdecnum_t *b1);
#endif /* KDECNUM_H */
