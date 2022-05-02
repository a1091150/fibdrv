
#include <inttypes.h>
#include <stdio.h>
#include "decnum.h"


#define PRINTDECNUM(a)                                            \
    printf("%" PRIu32, (a).digits[((a).size - 1)]);               \
    for (size_t i = 1; i < (a).size; i++) {                       \
        printf(",%09" PRIu32 "", (a).digits[((a).size - i - 1)]); \
    }                                                             \
    printf("\n");

int main()
{
    decnum_t b1 = DECNUM_INIT(1, 2);
    decnum_new(&b1);
    b1.digits[0] = DECMAXVALUE - 1;

    decnum_t b2 = DECNUM_INIT(1, 2);
    decnum_new(&b2);
    b2.digits[0] = DECMAXVALUE - 1;


    decnum_t res;
    decnum_mult(&b1, &b2, &res);
    PRINTDECNUM(b1);
    PRINTDECNUM(b2);
    PRINTDECNUM(res);
    decnum_free(&res);
    return 0;
}