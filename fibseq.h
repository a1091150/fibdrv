#ifndef FIBSEQ_H
#define FIBSEQ_H
#ifdef FIBINUSERSPACE
#include <stddef.h>
#else
#include <sys/types.h>
#endif

#include <limits.h>
enum fibmethod { basic = 0, basic_fast_doubling };

long long fibseq_basic(long long offset);

long long fibseq_basic_fast_doubling(long long offset);
#endif /* FIBSEQ_H */