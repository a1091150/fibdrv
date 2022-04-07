#include <stdio.h>
#include <time.h>
#include "fibseq.h"

#define NANOSECOND(x) ((x).tv_sec * 1e9 + (x).tv_nsec)
int main()
{
    for (int i = 0; i < 94; i++) {
        struct timespec t1, t2, t3;
        clock_gettime(CLOCK_MONOTONIC, &t1);
        long long ret1 = fibseq_basic_fast_doubling_branch(i);

        clock_gettime(CLOCK_MONOTONIC, &t2);
        long long ret2 = fibseq_basic_fast_doubling_branchless(i);
        clock_gettime(CLOCK_MONOTONIC, &t3);

        long long ut = (long long) (NANOSECOND(t2) - NANOSECOND(t1));
        long long ut2 = (long long) (NANOSECOND(t3) - NANOSECOND(t2));
        printf("%d %lld %lld %d\n", i, ut, ut2, ret1 == ret2);
    }
    return 0;
}