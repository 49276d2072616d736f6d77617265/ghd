#include "include/github.h"
#include <stdio.h>
#include <string.h>

int github_should_process(const webhook_event_t *ev)
{
    /* Ignora eventos sem ação */
    if (!ev->action[0])
        return 0;

    /* Só push por enquanto */
    if (strcmp(ev->action, "push") != 0)
        return 0;

    /* Só branch main */
    if (strcmp(ev->ref, "refs/heads/main") != 0)
        return 0;

    return 1;
}

void github_log_event(const webhook_event_t *ev)
{
    if (ev->action[0])
        printf("[ghd] action = %s\n", ev->action);

    if (ev->repo[0])
        printf("[ghd] repo   = %s\n", ev->repo);

    if (ev->ref[0])
        printf("[ghd] ref    = %s\n", ev->ref);
}
