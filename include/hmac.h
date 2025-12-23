#pragma once
#include <stddef.h>

int github_verify_hmac(const unsigned char *payload,
                       size_t payload_len,
                       const char *header_sig);
