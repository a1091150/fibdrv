#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"

#include "decnum.h"

#define PRINTDECNUM(a)                                            \
    printf("%" PRIu32, (a).digits[((a).size - 1)]);               \
    for (size_t i = 1; i < (a).size; i++) {                       \
        printf(",%09" PRIu32 "", (a).digits[((a).size - i - 1)]); \
    }                                                             \
    printf("\n");


#define BUFSIZE 8

int main()
{
    long long sz;

    int32_t buf[BUFSIZE];
    char write_buf[] = "testing writing";
    memset(buf, 0, sizeof(int32_t) * BUFSIZE);
    int offset = 100; /* TODO: try test something bigger than the limit */

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i < 99; i++) {
        lseek(fd, 50, SEEK_SET);
        sz = read(fd, buf, BUFSIZE * sizeof(int32_t));
        printf("%d %lld\n", i, sz);
    }
    decnum_t fib = DECNUM_INIT(BUFSIZE, BUFSIZE);
    fib.digits = buf;
    PRINTDECNUM(fib);

    close(fd);
    return 0;
}
