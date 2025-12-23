#define _GNU_SOURCE
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "include/config.h"
#include "include/http.h"
#include "include/hmac.h"
#include "include/json.h"

int main(void) {
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(LISTEN_PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(server, 5) < 0) {
        perror("listen");
        return 1;
    }

    printf("[ghd] listening on %d\n", LISTEN_PORT);

    for (;;) {
        int client = accept(server, NULL, NULL);
        if (client < 0)
            continue;

        char buf[16384];
        size_t header_len = 0;
        int content_length = 0;

        int received = http_read_request(
            client,
            buf,
            sizeof(buf),
            &header_len,
            &content_length
        );

        if (received < 0 ||
            content_length <= 0 ||
            content_length > MAX_BODY)
        {
            http_send_unauthorized(client);
            close(client);
            continue;
        }

        unsigned char *body = malloc(content_length);
        if (!body) {
            close(client);
            continue;
        }

        size_t already = received - header_len;
        if (already > (size_t)content_length)
            already = content_length;

        memcpy(body, buf + header_len, already);

        if (http_read_body(client, body, content_length, already) < 0) {
            free(body);
            close(client);
            continue;
        }

        char *sig = strcasestr(buf, "X-Hub-Signature-256:");
        int valid = 0;

        if (sig) {
            sig += strlen("X-Hub-Signature-256:");
            while (*sig == ' ')
                sig++;

            char *end = strstr(sig, "\r\n");
            if (end)
                *end = 0;

            valid = github_verify_hmac(body, content_length, sig);
        }

        if (!valid) {
            printf("[ghd] invalid webhook\n");
            http_send_unauthorized(client);
            free(body);
            close(client);
            continue;
        }

        printf("[ghd] valid webhook (%d bytes)\n", content_length);

        webhook_event_t ev;
        // if (parse_webhook_json(body, content_length, &ev) == 0) {
        //     if (ev.action[0])
        //         printf("[ghd] action = %s\n", ev.action);
        //     if (ev.repo[0])
        //         printf("[ghd] repo   = %s\n", ev.repo);
        //     if (ev.ref[0])
        //         printf("[ghd] ref    = %s\n", ev.ref);
        // } else {
        //     printf("[ghd] JSON parse error\n");
        // }

        if (parse_webhook_json(body, content_length, &ev) == 0) {
            github_log_event(&ev);

            if (!github_should_process(&ev)) {
                http_send_ok(client);
                free(body);
                close(client);
                continue;
            }
        } else {
            printf("[ghd] JSON parse error\n");
        }

        http_send_ok(client);

        free(body);
        close(client);
    }
}
