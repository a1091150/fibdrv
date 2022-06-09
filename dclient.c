
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
    decnum_t a1 = DECNUM_INIT(1, 8);
    decnum_new(&a1);
    a1.digits[0] = DECMAXVALUE - 1;
    decnum_t a2 = DECNUM_INIT(1, 8);
    decnum_new(&a2);
    a2.digits[0] = 1;

    decnum_t a3 = DECNUM_INIT(1, 1);
    decnum_new(&a3);

    decnum_t a4 = DECNUM_INIT(1, 8);
    decnum_new(&a4);

    decnum_add(&a1, &a2, &a3);
    decnum_add(&a1, &a2, &a4);

    PRINTDECNUM(a3);
    PRINTDECNUM(a4);
    return 0;
}