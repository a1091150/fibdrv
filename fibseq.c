
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
};

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
};