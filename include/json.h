#pragma once
#include <stddef.h>
#include "jsmn.h"

int jsoneq(const char *json, jsmntok_t *tok, const char *s);

typedef struct {
    char action[32];      // push, pull, etc
    char repo[128];       // sk/ghd
    char ref[64];         // refs/heads/main
    char exec_dir[256];   // diretório real a executar (opcional)

    char event_type[64];
    char user[64];
    char label[64];
} webhook_event_t;


int parse_webhook_json(const unsigned char *json,
                       size_t len,
                       webhook_event_t *out);
