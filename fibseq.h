#ifndef FIBSEQ_H
#define FIBSEQ_H

#include <limits.h>
#include <stddef.h>
#include "decnum.h"

long long fibseq_basic(int offset);

long long fibseq_basic_fast_doubling_branch(unsigned int offset);

long long fibseq_basic_fast_doubling_branchless(unsigned int offset);

void decnum_fast_doubling(unsigned int offset, decnum_t *result);
#endif /* FIBSEQ_H */