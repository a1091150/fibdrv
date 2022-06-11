#include "decnum.h"

void decnum_swap(decnum_t *b1, decnum_t *b2)
{
    uint32_t *tmp, size, cap;
    tmp = b1->digits;
    size = b1->size;
    cap = b1->cap;

    b1->digits = b2->digits;
    b1->size = b2->size;
    b1->cap = b2->cap;

    b2->digits = tmp;
    b2->size = size;
    b2->cap = cap;
}

void decnum_clean(decnum_t *b1)
{
    b1->size = 0;
    memset(b1->digits, 0, sizeof(int32_t) * b1->cap);
}

/*
    b1: destination
    b2: source
*/
void decnum_assign(decnum_t *b1, decnum_t *b2)
{
    if (!b1 || !b2 || b1 == b2) {
        return;
    }

    if (!b1->digits || !b2->digits) {
        return;
    }

    decnum_clean(b1);
    b1->size = min(b1->cap, b2->size);
    memcpy(b1->digits, b2->digits, sizeof(int32_t) * b1->size);
    memset(b1->digits + b1->size, 0, b1->cap - b1->size);
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

void decnum_add(decnum_t *b1, decnum_t *b2, decnum_t *result)
{
    if (!result || !b1 || !b2) {
        return;
    }

    if (b1->size > result->cap || b2->size > result->cap) {
        return;
    }

    decnum_t a1 = DECNUM_INIT(b1->size, b1->cap);
    a1.digits = b1->digits;
    decnum_t a2 = DECNUM_INIT(b2->size, b2->cap);
    a2.digits = b2->digits;
    if (b2->size > b1->size) {
        decnum_swap(&a1, &a2);
    }

    if (b1 != result && b2 != result) {
        decnum_clean(result);
    }

    int32_t carry = 0;
    for (size_t i = 0; i < a2.size; i++) {
        int32_t digit = a1.digits[i] + a2.digits[i] + carry;
        carry = digit >= DECMAXVALUE;
        result->digits[i] = digit % DECMAXVALUE;
    }

    if (carry) {
        result->size = a2.size;
        for (size_t i = a2.size; (i < a1.size) && carry; i++) {
            int32_t digit = a1.digits[i] + 1;
            result->size += 1;
            carry = digit >= DECMAXVALUE;
            result->digits[i] = digit % DECMAXVALUE;
        }

        if ((result->size < result->cap) && carry) {
            result->digits[result->size] = 1;
            result->size += 1;
        }
    } else {
        result->size = a1.size;
        memcpy(result->digits + a2.size, a1.digits + a2.size,
               sizeof(int32_t) * (a1.size - a2.size));
    }
}

/*
 * In fibonacci number, b1 is always greater than or equal to b2
 */
void decnum_sub(decnum_t *b1, decnum_t *b2, decnum_t *result)
{
    if (!result || !b1 || !b2) {
        return;
    }

    if (b1->size > result->cap || !b1->size || !b2->size) {
        return;
    }

    decnum_t a1 = DECNUM_INIT(b1->size, b1->cap);
    a1.digits = b1->digits;
    decnum_t a2 = DECNUM_INIT(b2->size, b2->cap);
    a2.digits = b2->digits;

    if (b1 != result && b2 != result) {
        decnum_clean(result);
    }

    int32_t carry = 0;
    for (size_t i = 0; i < a2.size; i++) {
        int32_t digit = a1.digits[i] - a2.digits[i] - carry;
        carry = digit < 0;
        result->digits[i] = digit + (DECMAXVALUE & -carry);
    }

    for (size_t i = a2.size; i < a1.size; i++) {
        int32_t digit = a1.digits[i] - carry;
        carry = digit < 0;
        result->digits[i] = digit + (DECMAXVALUE & -carry);
    }

    result->size = a1.size;
    size_t i = a1.size - 1;
    while (i && !result->digits[i]) {
        result->size--;
        i--;
    }
}

void decnum_mult(const decnum_t *b1, const decnum_t *b2, decnum_t *result)
{
    if (!result || !b1 || !b2) {
        return;
    }

    if ((b1->size + b2->size) > result->cap || !b1->size || !b2->size) {
        return;
    }

    decnum_clean(result);
    result->size = b1->size + b2->size;

    const int32_t size = b1->size + b2->size;

    for (size_t i = 0; i < b1->size; i++) {
        for (size_t j = 0; j < b2->size; j++) {
            int64_t carry =
                ((int64_t) b1->digits[i]) * ((int64_t) b2->digits[j]);
            size_t k = i + j;
            do {
                uint64_t vv = carry + result->digits[k];
                result->digits[k] = vv % DECMAXVALUE;
                carry = vv / DECMAXVALUE;
                k++;
            } while ((k < size) && carry);
        }
    }

    size_t i = result->size - 1;
    while (i && !result->digits[i]) {
        result->size--;
        i--;
    }
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

    if (carry && (b1->size < b1->cap)) {
        b1->digits[b1->size] = carry;
        b1->size++;
    }
}