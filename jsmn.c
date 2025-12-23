#include "include/jsmn.h"
#include <string.h>

void jsmn_init(jsmn_parser *parser) {
    parser->pos = 0;
    parser->toknext = 0;
    parser->toksuper = -1;
}

static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser,
                                   jsmntok_t *tokens,
                                   size_t num_tokens)
{
    if (parser->toknext >= num_tokens)
        return NULL;

    jsmntok_t *tok = &tokens[parser->toknext++];
    tok->start = -1;
    tok->end = -1;
    tok->size = 0;
    tok->parent = -1;
    return tok;
}

int jsmn_parse(jsmn_parser *parser,
               const char *js,
               size_t len,
               jsmntok_t *tokens,
               unsigned int num_tokens)
{
    for (; parser->pos < len; parser->pos++) {
        char c = js[parser->pos];
        jsmntok_t *tok;

        switch (c) {

        case '{':
        case '[':
            tok = jsmn_alloc_token(parser, tokens, num_tokens);
            if (!tok) return -1;

            tok->type = (c == '{') ? JSMN_OBJECT : JSMN_ARRAY;
            tok->start = parser->pos;
            tok->parent = parser->toksuper;
            parser->toksuper = parser->toknext - 1;
            break;

        case '}':
        case ']':
            for (int i = parser->toknext - 1; i >= 0; i--) {
                tok = &tokens[i];
                if (tok->start != -1 && tok->end == -1) {
                    tok->end = parser->pos + 1;
                    parser->toksuper = tok->parent;
                    break;
                }
            }
            break;

        case '\"':
            tok = jsmn_alloc_token(parser, tokens, num_tokens);
            if (!tok) return -1;

            tok->type = JSMN_STRING;
            tok->start = ++parser->pos;
            tok->parent = parser->toksuper;

            for (; parser->pos < len; parser->pos++) {
                if (js[parser->pos] == '\"') {
                    tok->end = parser->pos;
                    break;
                }
            }
            break;

        /* IGNORAR WHITESPACE E SEPARADORES JSON */
        case '\t':
        case '\r':
        case '\n':
        case ' ':
        case ':':
        case ',':
            break;

        default:
            tok = jsmn_alloc_token(parser, tokens, num_tokens);
            if (!tok) return -1;

            tok->type = JSMN_PRIMITIVE;
            tok->start = parser->pos;
            tok->parent = parser->toksuper;

            for (; parser->pos < len; parser->pos++) {
                if (strchr(",}] \t\r\n", js[parser->pos])) {
                    tok->end = parser->pos;
                    parser->pos--;
                    break;
                }
            }

            if (tok->end == -1)
                tok->end = parser->pos + 1;

            break;
        }
    }

    return parser->toknext;
}
