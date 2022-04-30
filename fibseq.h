#ifndef FIBSEQ_H
#define FIBSEQ_H

#include <limits.h>
#include <stddef.h>

long long fibseq_basic(int offset);

long long fibseq_basic_fast_doubling_branch(unsigned int offset);

long long fibseq_basic_fast_doubling_branchless(unsigned int offset);
#endif /* FIBSEQ_H */