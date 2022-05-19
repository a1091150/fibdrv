#include "kdecnum.h"

void kdecnum_swap(kdecnum_t *b1, kdecnum_t *b2)
{
    __u32 *tmp, size;
    tmp = b1->digits;
    size = b1->size;
    b1->digits = b2->digits;
    b1->size = b2->size;
    b2->digits = tmp;
    b2->size = size;
}

static inline void kdecnum_restore_size(kdecnum_t *result)
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

void kdecnum_new(kdecnum_t *ptr)
{
    if (!ptr) {
        return;
    }
    ptr->digits = kmalloc(sizeof(__u32) * ptr->cap, GFP_KERNEL);
    ptr->cap = ptr->digits ? ptr->cap : 0;
    ptr->size = ptr->digits ? ptr->size : 0;
}

void kdecnum_free(kdecnum_t *ptr)
{
    if (!ptr) {
        return;
    }
    kfree(ptr->digits);
}

void kdecnum_add(const kdecnum_t *b1, const kdecnum_t *b2, kdecnum_t *result)
{
    if (!result && !b1 && !b2) {
        return;
    }

    kdecnum_t a1 = KDECNUM_INIT(b1->size, b1->cap);
    a1.digits = b1->digits;
    kdecnum_t a2 = KDECNUM_INIT(b2->size, b2->cap);
    a2.digits = b2->digits;
    if (b2->size > b1->size) {
        kdecnum_swap(&a1, &a2);
    }

    result->size = max(a1.size, a2.size);
    result->cap = result->size + 1;
    kdecnum_new(result);

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
void kdecnum_sub(const kdecnum_t *b1, const kdecnum_t *b2, kdecnum_t *result)
{
    if (!result && !b1 && !b2) {
        return;
    }

    kdecnum_t a1 = KDECNUM_INIT(b1->size, b1->cap);
    a1.digits = b1->digits;
    kdecnum_t a2 = KDECNUM_INIT(b2->size, b2->cap);
    a2.digits = b2->digits;

    result->size = max(a1.size, a2.size);
    result->cap = result->size;
    kdecnum_new(result);

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

    kdecnum_restore_size(result);
}

void kdecnum_mult(const kdecnum_t *b1, const kdecnum_t *b2, kdecnum_t *result)
{
    if (!result && !b1 && !b2) {
        return;
    }

    result->size = b1->size + b2->size;
    result->cap = result->size;
    kdecnum_new(result);
    const int32_t size = b1->size + b2->size;
    int64_t *tmp = kmalloc(sizeof(int64_t) * size, GFP_KERNEL);
    memset(tmp, 0, sizeof(int64_t) * size);

    for (size_t i = 0; i < b1->size; i++) {
        for (size_t j = 0; j < b2->size; j++) {
            tmp[(i + j)] +=
                ((int64_t) b1->digits[i]) * ((int64_t) b2->digits[j]);
        }
    }

    __u64 carry = 0;
    for (size_t i = 0; i < size; i++) {
        tmp[i] += carry;
        carry = tmp[i] / DECMAXVALUE;
        tmp[i] %= DECMAXVALUE;
    }

    for (size_t i = 0; i < size; i++) {
        result->digits[i] = tmp[i];
    }
    kfree(tmp);
    kdecnum_restore_size(result);
}


void kdecnum_multi_by_two(kdecnum_t *b1)
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
            b1->digits =
                krealloc(b1->digits, sizeof(__u32) * b1->cap, GFP_KERNEL);
            b1->digits[(b1->size - 1)] = carry;
        }
    }
}