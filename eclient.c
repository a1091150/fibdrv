#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"

#define SIZEOFBUF 500

int main()
{
    long long sz;

    char buf[SIZEOFBUF];
    char write_buf[] = "testing writing";
    memset(buf, 0, sizeof(char) * 500);
    int offset = 100; /* TODO: try test something bigger than the limit */

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    lseek(fd, 6, SEEK_SET);
    sz = read(fd, buf, SIZEOFBUF);

    close(fd);
    return 0;
}
