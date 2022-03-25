#ifndef FIBSEQ_H
#define FIBSEQ_H

#include <stddef.h>
// #include <sys/types.h>

#include <limits.h>
#include "bn_kernel.h"
enum fibmethod {
    iter_basic = 0,
    iter_fast_doubling,
    bn_basic,
    bn_fast_doubling
};

long long fibseq_basic(long long offset);

long long fibseq_basic_fast_doubling(long long offset);

char *fibseq_basic_bn(long long offset);
#endif /* FIBSEQ_H */