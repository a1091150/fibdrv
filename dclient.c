
#include <inttypes.h>
#include <stdio.h>
#include "decnum.h"
#include "fibseq.h"


#define PRINTDECNUM(a)                                            \
    printf("%" PRIu32, (a).digits[((a).size - 1)]);               \
    for (size_t i = 1; i < (a).size; i++) {                       \
        printf(",%09" PRIu32 "", (a).digits[((a).size - i - 1)]); \
    }                                                             \
    printf("\n");

int main()
{
    decnum_t fib = DECNUM_INIT(0, 0);

    for (size_t i = 0; i < 100; i++) {
        decnum_fast_doubling(i, &fib);
        printf("%lu ", i);
        PRINTDECNUM(fib);
        decnum_free(&fib);
    }
    return 0;
}