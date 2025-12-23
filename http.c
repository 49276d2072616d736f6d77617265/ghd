#define _GNU_SOURCE
#include "include/http.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>

int http_read_request(int fd,
                      char *buf,
                      size_t buf_size,
                      size_t *header_len,
                      int *content_length)
{
    int received = 0;

    while (received < (int)buf_size - 1) {
        int n = recv(fd, buf + received, buf_size - 1 - received, 0);
        if (n <= 0)
            return -1;
        received += n;
        buf[received] = 0;
        if (strstr(buf, "\r\n\r\n"))
            break;
    }

    char *end = strstr(buf, "\r\n\r\n");
    if (!end)
        return -1;

    *header_len = (end + 4) - buf;

    char *cl = strcasestr(buf, "Content-Length:");
    *content_length = cl ? atoi(cl + 15) : 0;

    return received;
}

int http_read_body(int fd,
                   unsigned char *body,
                   size_t content_length,
                   size_t already_read)
{
    size_t read = already_read;

    while (read < content_length) {
        int n = recv(fd, body + read, content_length - read, 0);
        if (n <= 0)
            return -1;
        read += n;
    }
    return 0;
}

void http_send_ok(int fd) {
    const char *resp =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 2\r\n\r\nOK";
    write(fd, resp, strlen(resp));
}

void http_send_unauthorized(int fd) {
    const char *resp =
        "HTTP/1.1 401 Unauthorized\r\n"
        "Content-Length: 0\r\n\r\n";
    write(fd, resp, strlen(resp));
}
