#include "fibseq.h"

long long fibseq_basic(long long offset)
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

long long fibseq_basic_fast_doubling(long long offset)
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

char *fibseq_basic_bn(long long offset)
{
    struct bn *a = bn_new(1);
    a->number[0] = 0;
    struct bn *b = bn_new(1);
    b->number[0] = 1;

    struct bn *c = bn_add(a, b);
    char *result = bn_to_string(c);

    struct bn *d = bn_mult(a, b);
    struct bn *e = bn_square(a);
    struct bn *f = bn_sub(a, b);
    bn_multbytwo(a);
    bn_free(a);
    bn_free(b);
    bn_free(c);
    bn_free(d);
    bn_free(e);
    bn_free(f);
    return result;
}