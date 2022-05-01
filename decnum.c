#include "decnum.h"
void decnum_new(decnum_t *ptr, uint32_t size)
{
    if (!ptr) {
        return;
    }
    ptr->digits = malloc(sizeof(uint32_t) * size);
}

void decnum_free(decnum_t *ptr)
{
    if (!ptr) {
        return;
    }
    free(ptr->digits);
}

void decnum_add(const decnum_t *b1, const decnum_t *b2, decnum_t *result) {
    if(!result && !b1 && !b2){
        return;
    }
}

void decnum_sub(const decnum_t *b1, const decnum_t *b2, decnum_t *result) {
    if(!result && !b1 && !b2){
        return;
    }
}

void decnum_mult(const decnum_t *b1, const decnum_t *b2, decnum_t *result) {
    if(!result && !b1 && !b2){
        return;
    }
}