#ifndef DECNUM_H
#define DECNUM_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
struct decnum {
    uint32_t size;
    uint32_t cap;
    int32_t *digits;
};

#define DECNUM_INIT(a, b)                       \
    {                                           \
        .size = (a), .cap = (b), .digits = NULL \
    }

typedef struct decnum decnum_t;

#define DECMAXVALUE 1000000000  // 10^9
#define max(a, b)              \
    ({                         \
        typeof(a) __a = (a);   \
        typeof(b) __b = (b);   \
        __a > __b ? __a : __b; \
    })

#define min(a, b)              \
    ({                         \
        typeof(a) __a = (a);   \
        typeof(b) __b = (b);   \
        __a > __b ? __b : __a; \
    })


void decnum_new(decnum_t *ptr);
void decnum_free(decnum_t *ptr);
void decnum_swap(decnum_t *b1, decnum_t *b2);
void decnum_add(decnum_t *b1, decnum_t *b2, decnum_t *result);
void decnum_sub(decnum_t *b1, decnum_t *b2, decnum_t *result);
void decnum_mult(const decnum_t *b1, const decnum_t *b2, decnum_t *result);
void decnum_multi_by_two(decnum_t *b1);
#endif /* DECNUM_H */
