#pragma once
#include <stddef.h>

int http_read_request(int fd,
                      char *buf,
                      size_t buf_size,
                      size_t *header_len,
                      int *content_length);

int http_read_body(int fd,
                   unsigned char *body,
                   size_t content_length,
                   size_t already_read);

void http_send_ok(int fd);
void http_send_unauthorized(int fd);
