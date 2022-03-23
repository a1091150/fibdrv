#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"

#define FIBINUSERSPACE
/* If gcc -D FIBINUSERSPACE is set, program use userspace function code to test
 * fib sequence. */
#ifdef FIBINUSERSPACE
#include "fibseq.h"
#endif


void test_in_userspace(enum fibmethod m)
{
    long long (*fptr[2])(long long) = {fibseq_basic,
                                       fibseq_basic_fast_doubling};
    const int offset = 100;

    for (int i = 0; i <= offset; i++) {
        printf("Writing to " FIB_DEV ", returned the sequence %d\n", 1);
    }

    for (int i = 0; i <= offset; i++) {
        long long sz = fptr[m](i);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%lld.\n",
               i, sz);
    }

    for (int i = offset; i >= 0; i--) {
        long long sz = fptr[m](i);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%lld.\n",
               i, sz);
    }
}

int main()
{
#ifdef FIBINUSERSPACE
    test_in_userspace(basic_fast_doubling);
    return 0;
#endif

    long long sz;

    char buf[1];
    char write_buf[] = "testing writing";
    int offset = 100; /* TODO: try test something bigger than the limit */

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        sz = write(fd, write_buf, strlen(write_buf));
        printf("Writing to " FIB_DEV ", returned the sequence %lld\n", sz);
    }

    for (int i = 0; i <= offset; i++) {
        lseek(fd, i, SEEK_SET);
        sz = read(fd, buf, 1);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%lld.\n",
               i, sz);
    }

    for (int i = offset; i >= 0; i--) {
        lseek(fd, i, SEEK_SET);
        sz = read(fd, buf, 1);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%lld.\n",
               i, sz);
    }

    close(fd);
    return 0;
}
