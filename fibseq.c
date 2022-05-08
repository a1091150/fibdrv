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

    unsigned long long mask = ULLONG_MAX ^ (ULLONG_MAX >> 1);
    mask >>= __builtin_clz(offset);

    decnum_t a = DECNUM_INIT(1, 1);
    decnum_new(&a);
    a.digits[0] = 0;
    decnum_t b = DECNUM_INIT(1, 1);
    decnum_new(&b);
    b.digits[0] = 1;

    for (; mask; mask >>= 1) {
        if (mask & offset) {
            decnum_t c = DECNUM_INIT(0, 0);
            decnum_t d = DECNUM_INIT(0, 0);

            decnum_t aq = DECNUM_INIT(0, 0);
            decnum_t bq = DECNUM_INIT(0, 0);

            decnum_t ab = DECNUM_INIT(0, 0);

            decnum_mult(&a, &a, &aq);
            decnum_mult(&b, &b, &bq);

            decnum_mult(&a, &b, &ab);
            decnum_multi_by_two(&ab);
            decnum_add(&ab, &bq, &c);

            decnum_add(&aq, &bq, &d);

            // a = a * a + b * b
            decnum_swap(&a, &d);

            // b = a * (2 * b - a) + a * a + b * b =  2 * a * b + b * b
            decnum_swap(&b, &c);

            decnum_free(&c);
            decnum_free(&d);
            decnum_free(&ab);
            decnum_free(&aq);
            decnum_free(&bq);
        } else {
            decnum_t c = DECNUM_INIT(0, 0);
            decnum_t d = DECNUM_INIT(0, 0);

            decnum_t aq = DECNUM_INIT(0, 0);
            decnum_t bq = DECNUM_INIT(0, 0);

            decnum_t ab = DECNUM_INIT(0, 0);

            decnum_mult(&a, &a, &aq);
            decnum_mult(&b, &b, &bq);
            decnum_add(&aq, &bq, &d);

            decnum_multi_by_two(&b);
            decnum_mult(&a, &b, &ab);
            decnum_sub(&ab, &aq, &c);

            // a = 2ab - a^2
            decnum_swap(&a, &c);
            // b = a^2 + b^2
            decnum_swap(&b, &d);

            decnum_free(&c);
            decnum_free(&d);
            decnum_free(&aq);
            decnum_free(&bq);
        }
    }

    decnum_swap(&a, result);
    decnum_free(&b);
}