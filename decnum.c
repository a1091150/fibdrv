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

static inline void decnum_restore_size(decnum_t *result)
{
    int ischanged = 0;
    for (size_t i = 0; i < result->size; i++) {
        if (result->digits[(result->size - i - 1)]) {
            result->size = result->size - i;
            ischanged = 1;
            break;
        }
    }

    if (!ischanged && !result->digits[0]) {
        result->size = 1;
    }
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

    int32_t carry = 0;
    for (size_t i = 0; i < a2.size; i++) {
        int32_t digit = a1.digits[i] + a2.digits[i] + carry;
        carry = digit >= DECMAXVALUE;
        result->digits[i] = digit % DECMAXVALUE;
    }

    for (size_t i = a2.size; i < a1.size; i++) {
        int32_t digit = a1.digits[i] + carry;
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

/*
 * In fibonacci number, b1 is always greater than or equal to b2
 */
void decnum_sub(const decnum_t *b1, const decnum_t *b2, decnum_t *result)
{
    if (!result && !b1 && !b2) {
        return;
    }

    decnum_t a1 = DECNUM_INIT(b1->size, b1->cap);
    a1.digits = b1->digits;
    decnum_t a2 = DECNUM_INIT(b2->size, b2->cap);
    a2.digits = b2->digits;

    result->size = max(a1.size, a2.size);
    result->cap = result->size;
    decnum_new(result);

    int32_t carry = 0;
    for (size_t i = 0; i < a2.size; i++) {
        int32_t digit = a1.digits[i] - a2.digits[i] - carry;
        if (digit < 0) {
            carry = 1;
            result->digits[i] = digit + DECMAXVALUE;
        } else {
            carry = 0;
            result->digits[i] = digit;
        }
    }

    for (size_t i = a2.size; i < a1.size; i++) {
        int32_t digit = a1.digits[i] - carry;
        if (digit < 0) {
            carry = 1;
            result->digits[i] = digit + DECMAXVALUE;
        } else {
            carry = 0;
            result->digits[i] = digit;
        }
    }

    decnum_restore_size(result);
}

void decnum_mult(const decnum_t *b1, const decnum_t *b2, decnum_t *result)
{
    if (!result && !b1 && !b2) {
        return;
    }

    result->size = b1->size + b2->size;
    result->cap = result->size;
    decnum_new(result);
    const int32_t size = b1->size + b2->size;
    int64_t *tmp = malloc(sizeof(int64_t) * size);

    for (size_t i = 0; i < b1->size; i++) {
        for (size_t j = 0; j < b2->size; j++) {
            tmp[(i + j)] +=
                ((int64_t) b1->digits[i]) * ((int64_t) b2->digits[j]);
        }
    }

    uint64_t carry = 0;
    for (size_t i = 0; i < size; i++) {
        tmp[i] += carry;
        carry = tmp[i] / DECMAXVALUE;
        tmp[i] %= DECMAXVALUE;
    }

    for (size_t i = 0; i < size; i++) {
        result->digits[i] = tmp[i];
    }
    free(tmp);
    decnum_restore_size(result);
}


void decnum_multi_by_two(decnum_t *b1)
{
    if (!b1) {
        return;
    }

    for (size_t i = 0; i < b1->size; i++) {
        b1->digits[i] <<= 1;
    }

    int32_t carry = 0;
    for (size_t i = 0; i < b1->size; i++) {
        int32_t digit = b1->digits[i] + carry;
        b1->digits[i] = digit % DECMAXVALUE;
        carry = digit / DECMAXVALUE;
    }

    if (carry) {
        b1->size++;
        if (b1->size < b1->cap) {
            b1->digits[(b1->size - 1)] = carry;
        } else {
            b1->cap = b1->size;
            b1->digits = realloc(b1->digits, sizeof(uint32_t) * b1->cap);
            b1->digits[(b1->size - 1)] = carry;
        }
    }
}