#ifndef DECNUM_H
#define DECNUM_H

#include <stdint.h>
struct decnum {
    uint32_t size;
    uint32_t *digits;
};

#define DECNUM_INIT                 \
    {                               \
        {                           \
            .size = 0, .digits = 0, \
        }                           \
    }

typedef decnum decnum_t;


void decnum_new(decnum_t *ptr, uint32_t size);
void decnum_free(decnum_t *ptr);

void decnum_add(const decnum_t *b1, const decnum_t *b2, decnum_t *result);
void decnum_sub(const decnum_t *b1, const decnum_t *b2, decnum_t *result);
void decnum_mult(const decnum_t *b1, const decnum_t *b2, decnum_t *result);
#endif /* DECNUM_H */
