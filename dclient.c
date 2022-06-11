
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

void reverse(int32_t *arr, const size_t size)
{
    int32_t tmp = 0;
    const size_t ss = size >> 1;
    for (size_t i = 0; i < ss; i++) {
        tmp = arr[i];
        arr[i] = arr[(size - i - 1)];
        arr[(size - i - 1)] = tmp;
    }
}

int main()
{
    // Test fib(521)
    int32_t ar[14] = {0,         0,         0,         0,         0,
                      0,         0,         0,         971183874, 599339129,
                      547649988, 289594072, 811608739, 584170445};
    reverse(ar, 14);
    decnum_t a = DECNUM_INIT(6, 14);
    a.digits = ar;

    int32_t br[14] = {0,         0,         0,         0,         0,
                      0,         0,         1,         571408518, 427546378,
                      167846658, 524186148, 133445300, 987550786};
    reverse(br, 14);
    decnum_t b = DECNUM_INIT(7, 14);
    b.digits = br;

    int32_t tr[14];
    decnum_t tmp = DECNUM_INIT(0, 14);
    tmp.digits = tr;
    decnum_clean(&tmp);

    int32_t tr2[14];
    decnum_t tmp2 = DECNUM_INIT(0, 14);
    tmp2.digits = tr;
    decnum_clean(&tmp2);

    decnum_add(&a, &b, &tmp);
    PRINTDECNUM(tmp);

    decnum_t fib = DECNUM_INIT(0, 0);
    decnum_fast_doubling(521, &fib);
    PRINTDECNUM(fib);
    return 0;
}