#include "include/json.h"
#include "include/jsmn.h"
#include "include/utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    return tok->type == JSMN_STRING &&
           (int)strlen(s) == tok->end - tok->start &&
           strncmp(json + tok->start, s, tok->end - tok->start) == 0;
}

int parse_webhook_json(const unsigned char *json,
                       size_t len,
                       webhook_event_t *out)
{
    fprintf(stderr, "[json] raw payload (%zu bytes):\n", len);
    fwrite(json, 1, len, stderr);
    fprintf(stderr, "\n");

    jsmn_parser p;
    jsmntok_t tokens[256];

    memset(out, 0, sizeof(*out));

    jsmn_init(&p);
    int r = jsmn_parse(&p, (const char *)json, len, tokens, 256);
    if (r < 0) {
        fprintf(stderr, "[json] jsmn parse failed: %d\n", r);
        return -1;
    }

    int repo_obj = -1;
    int pr_obj = -1;
    int issue_obj = -1;

    for (int i = 1; i < r; i++) {
        /* action */
        if (jsoneq((char *)json, &tokens[i], "action")) {
            jsmntok_t *v = &tokens[i + 1];
            snprintf(out->action, sizeof(out->action),
                     "%.*s", v->end - v->start, json + v->start);
            i++;
            continue;
        }

        /* ref */
        if (jsoneq((char *)json, &tokens[i], "ref")) {
            jsmntok_t *v = &tokens[i + 1];
            snprintf(out->ref, sizeof(out->ref),
                     "%.*s", v->end - v->start, json + v->start);
            i++;
            continue;
        }

        /* repository object */
        if (jsoneq((char *)json, &tokens[i], "repository")) {
            repo_obj = i + 1; /* object token */
            i++;
            continue;
        }

        /* repository.full_name */
        if (repo_obj != -1 &&
            tokens[i].parent == repo_obj &&
            jsoneq((char *)json, &tokens[i], "full_name")) {
            jsmntok_t *v = &tokens[i + 1];
            snprintf(out->repo, sizeof(out->repo),
                     "%.*s", v->end - v->start, json + v->start);
            i++;
            continue;
        }

        /* exec_dir */
        if (jsoneq((char *)json, &tokens[i], "exec_dir")) {
            jsmntok_t *v = &tokens[i + 1];
            snprintf(out->exec_dir, sizeof(out->exec_dir),
                     "%.*s", v->end - v->start, json + v->start);
            trim(out->exec_dir);
            i++;
            continue;
        }

        /* pull_request object */
        if (jsoneq((char *)json, &tokens[i], "pull_request") &&
            tokens[i + 1].type == JSMN_OBJECT) {
            pr_obj = i + 1;
            // Não incrementamos i aqui, vamos processar os campos abaixo
            i++; // Pular para o objeto
            continue;
        }

        /* issue object */
        if (jsoneq((char *)json, &tokens[i], "issue") &&
            tokens[i + 1].type == JSMN_OBJECT) {
            issue_obj = i + 1;
            i++; // Pular para o objeto
            continue;
        }

        /* pull_request.number */
        if (pr_obj != -1 && tokens[i].parent == pr_obj &&
            jsoneq((char *)json, &tokens[i], "number")) {
            jsmntok_t *v = &tokens[i + 1];
            int pr_number = 0;
            sscanf(json + v->start, "%d", &pr_number);
            snprintf(out->ref, sizeof(out->ref), "pr/%d", pr_number);
            i++;
            continue;
        }

        /* issue.number */
        if (issue_obj != -1 && tokens[i].parent == issue_obj &&
            jsoneq((char *)json, &tokens[i], "number")) {
            jsmntok_t *v = &tokens[i + 1];
            int issue_number = 0;
            sscanf(json + v->start, "%d", &issue_number);
            snprintf(out->ref, sizeof(out->ref), "issue/%d", issue_number);
            i++;
            continue;
        }
    }

    // Debug: imprimir os valores parseados
    fprintf(stderr, "[json] parsed: action=%s, repo=%s, ref=%s, exec_dir=%s\n",
            out->action, out->repo, out->ref, out->exec_dir);

    return 0;
}