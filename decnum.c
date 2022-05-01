#include "decnum.h"

static inline void decnum_swap(decnum_t *b1, decnum_t *b2)
{
    uint32_t *tmp, size;
    tmp = b1->digits;
    size = b1->size;
    b1->digits = b2->digits;
    b1->size = b2->size;
    b2->digits = tmp;
    b2->size = size;
}

void decnum_new(decnum_t *ptr)
{
    if (!ptr) {
        return;
    }
    ptr->digits = malloc(sizeof(uint32_t) * ptr->cap);
    ptr->cap = ptr->digits ? ptr->cap : 0;
    ptr->size = ptr->digits ? ptr->size : 0;
}

void decnum_free(decnum_t *ptr)
{
    if (!ptr) {
        return;
    }
    free(ptr->digits);
}

void decnum_add(const decnum_t *b1, const decnum_t *b2, decnum_t *result)
{
    if (!result && !b1 && !b2) {
        return;
    }

    decnum_t a1 = DECNUM_INIT(b1->size, b1->cap);
    a1.digits = b1->digits;
    decnum_t a2 = DECNUM_INIT(b2->size, b2->cap);
    a2.digits = b2->digits;
    if (b2->size > b1->size) {
        decnum_swap(&a1, &a2);
    }

    result->size = max(a1.size, a2.size);
    result->cap = result->size + 1;
    decnum_new(result);

    uint32_t carry = 0;
    for (size_t i = 0; i < a2.size; i++) {
        uint32_t digit = a1.digits[i] + a2.digits[i] + carry;
        carry = digit >= DECMAXVALUE;
        result->digits[i] = digit % DECMAXVALUE;
    }

    for (size_t i = a2.size; i < a1.size; i++) {
        uint32_t digit = a1.digits[i] + carry;
        carry = digit >= DECMAXVALUE;
        result->digits[i] = digit % DECMAXVALUE;
        if (!carry) {
            result->size = i + 1;
            break;
        }
    }

    if (carry) {
        result->digits[result->size] = 1;
        result->size++;
    }
}

void decnum_sub(const decnum_t *b1, const decnum_t *b2, decnum_t *result)
{
    if (!result && !b1 && !b2) {
        return;
    }
}

void decnum_mult(const decnum_t *b1, const decnum_t *b2, decnum_t *result)
{
    if (!result && !b1 && !b2) {
        return;
    }
}