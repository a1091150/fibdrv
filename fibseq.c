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
}