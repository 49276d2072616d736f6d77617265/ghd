#pragma once
#include <stddef.h>

typedef struct {
    char action[32];
    char repo[128];
    char ref[128];
} webhook_event_t;

int parse_webhook_json(const unsigned char *json,
                       size_t len,
                       webhook_event_t *out);
