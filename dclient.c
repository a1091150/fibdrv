
#include <inttypes.h>
#include <stdio.h>
#include "decnum.h"


#define PRINTDECNUM(a)                                           \
    printf("%" PRIu32, (a).digits[((a).size - 1)]);              \
    for (size_t i = 1; i < (a).size; i++) {                      \
        printf("%06" PRIu32 "", (a).digits[((a).size - i - 1)]); \
    }                                                            \
    printf("\n");

int main()
{
    decnum_t b1 = DECNUM_INIT(1, 1);
    decnum_new(&b1);


    decnum_t b2 = DECNUM_INIT(1, 1);
    decnum_new(&b2);
    b2.digits[0] = 100 - 1;

    decnum_t res;

    uint32_t test_data[6] = {
        (DECMAXVALUE - 1), (DECMAXVALUE - 1), 99, 99, 0, 1};
    for (int z = 0; z < 6; z += 2) {
        b1.digits[0] = test_data[z];
        b2.digits[0] = test_data[(z + 1)];

        decnum_add(&b1, &b2, &res);
        PRINTDECNUM(res);
        decnum_free(&res);
    }

    return 0;
}