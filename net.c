#include <unistd.h>
#include <errno.h>

ssize_t write_all(int fd, const void *buf, size_t len) {
    const char *p = buf;
    while (len > 0) {
        ssize_t r = write(fd, p, len);
        if (r <= 0) return -1;
        p += r;
        len -= r;
    }
    return 0;
}
