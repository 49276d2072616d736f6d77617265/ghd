#include "include/hmac.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <string.h>

#include "include/config.h"

int github_verify_hmac(const unsigned char *payload,
                       size_t payload_len,
                       const char *header_sig)

{
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int len = 0;

    HMAC(EVP_sha256(),
         GITHUB_HMAC_SECRET, strlen(GITHUB_HMAC_SECRET),
         payload, payload_len,
         digest, &len);

    char hex[EVP_MAX_MD_SIZE * 2 + 1];
    for (unsigned int i = 0; i < len; i++)
        sprintf(hex + i * 2, "%02x", digest[i]);
    hex[len * 2] = 0;

    if (strncmp(header_sig, "sha256=", 7) != 0)
        return 0;

    return strcmp(hex, header_sig + 7) == 0;
}
