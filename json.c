#include "include/json.h"
#include "include/jsmn.h"
#include <string.h>
#include <stdio.h>

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    return tok->type == JSMN_STRING &&
           (int)strlen(s) == tok->end - tok->start &&
           strncmp(json + tok->start, s, tok->end - tok->start) == 0;
}

int parse_webhook_json(const unsigned char *json,
                       size_t len,
                       webhook_event_t *out)
{
    jsmn_parser p;
    jsmntok_t tokens[256];

    memset(out, 0, sizeof(*out));

    jsmn_init(&p);
    int r = jsmn_parse(&p, (const char *)json, len, tokens, 256);
    if (r < 0)
        return -1;

    for (int i = 1; i < r; i++) {

        if (jsoneq((char *)json, &tokens[i], "action")) {
            jsmntok_t *v = &tokens[i + 1];
            snprintf(out->action, sizeof(out->action),
                     "%.*s", v->end - v->start, json + v->start);
            i++;
        }

        if (jsoneq((char *)json, &tokens[i], "ref")) {
            jsmntok_t *v = &tokens[i + 1];
            snprintf(out->ref, sizeof(out->ref),
                     "%.*s", v->end - v->start, json + v->start);
            i++;
        }

        if (jsoneq((char *)json, &tokens[i], "full_name")) {
            jsmntok_t *v = &tokens[i + 1];
            snprintf(out->repo, sizeof(out->repo),
                     "%.*s", v->end - v->start, json + v->start);
            i++;
        }
    }

    return 0;
}
