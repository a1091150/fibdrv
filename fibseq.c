
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
    unsigned long long h = 0;
    for (int i = offset; i; i >>= 1, h++)
        ;

    unsigned long long a = 0, b = 1;  // fib(0), fib(1)

    for (unsigned long long mask = 1 << (h - 1); mask; mask >>= 1) {
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
};