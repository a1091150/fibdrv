#include "fibseq.h"

long long fibseq_basic(int offset)
{
    if (!offset) {
        return 0;
    }

    long long y = 1, z = 1;

    for (int i = 2; i < offset; i++) {
        long long tmp = y;
        y = z;
        z = y + tmp;
    }

    return z;
}

long long fibseq_basic_fast_doubling_branchless(unsigned int offset)
{
    unsigned long long mask = ULLONG_MAX ^ (ULLONG_MAX >> 1);
    mask >>= __builtin_clz(offset);

    unsigned long long a = 0, b = 1;  // fib(0), fib(1)
    for (; mask; mask >>= 1) {
        unsigned long long c = a * (2 * b - a);
        unsigned long long d = a * a + b * b;

        int aset = !(mask & offset);
        int bset = !!(mask & offset);
        a = d ^ ((c ^ d) & -aset);
        b = d + (c & -bset);
    }

    return a;
}


long long fibseq_basic_fast_doubling_branch(unsigned int offset)
{
    unsigned long long mask = ULLONG_MAX ^ (ULLONG_MAX >> 1);
    mask >>= __builtin_clz(offset);

    unsigned long long a = 0, b = 1;  // fib(0), fib(1)
    for (; mask; mask >>= 1) {
        unsigned long long c = a * (2 * b - a);
        unsigned long long d = a * a + b * b;
        if (mask & offset) {
            a = d;
            b = c + d;
        } else {
            a = c;
            b = d;
        }
    }

    return a;
}

void decnum_fast_doubling(unsigned int offset, decnum_t *result)
{
    if (!result) {
        return;
    }
    const float factor = 42.25;
    unsigned int sp = (((float) offset) / factor) + 2;

    decnum_t a = DECNUM_INIT(1, sp);
    decnum_new(&a);

    if (!a.digits) {
        goto fa;
    }

    decnum_t b = DECNUM_INIT(1, sp);
    decnum_new(&b);

    if (!b.digits) {
        goto fb;
    }

    decnum_t tmp = DECNUM_INIT(1, sp);
    decnum_new(&tmp);

    if (!tmp.digits) {
        goto ft;
    }

    decnum_t tmp2 = DECNUM_INIT(1, sp);
    decnum_new(&tmp2);

    if (!tmp2.digits) {
        goto ft2;
    }

    decnum_clean(&a);
    decnum_clean(&b);
    decnum_clean(&tmp);
    decnum_clean(&tmp2);

    a.size = 1;
    a.digits[0] = 0;
    b.size = 1;
    b.digits[0] = 1;

    unsigned long long mask = ULLONG_MAX ^ (ULLONG_MAX >> 1);
    mask >>= __builtin_clz(offset);


    for (; mask; mask >>= 1) {
        if (mask & offset) {
            // A = (a + b)^2 - 2ab = a^2 + b^2
            decnum_add(&a, &b, &tmp);
            decnum_mult(&tmp, &tmp, &tmp2);
            decnum_mult(&a, &b, &tmp);
            decnum_multi_by_two(&tmp);
            decnum_sub(&tmp2, &tmp, &tmp);

            // B = (a + b)^2 - a^2
            decnum_mult(&a, &a, &b);
            decnum_sub(&tmp2, &b, &b);

            decnum_assign(&a, &tmp);
        } else {
            // B = a^2 + b^2
            decnum_mult(&a, &a, &tmp);
            decnum_mult(&b, &b, &tmp2);
            decnum_add(&tmp, &tmp2, &tmp2);

            // A = a * (2 * b - a)
            decnum_multi_by_two(&b);
            decnum_sub(&b, &a, &b);
            decnum_mult(&b, &a, &tmp);
            decnum_assign(&b, &tmp2);

            decnum_assign(&a, &tmp);
        }
    }

    decnum_free(&tmp);
    decnum_free(&tmp2);
    decnum_free(&b);
    decnum_swap(result, &a);
    return;

ft2:
    decnum_free(&tmp);
ft:
    decnum_free(&b);
fb:
    decnum_free(&a);
fa:
    result->cap = 0;
    result->size = 0;
    result->digits = NULL;
}