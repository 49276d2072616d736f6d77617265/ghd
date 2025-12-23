#include "include/github.h"
#include "include/utils.h"
#include <stdio.h>
#include <string.h>

ghp_status_t github_should_process(webhook_event_t *ev) {
    // Só aceita push
    if (!ev->action[0] || strcmp(ev->action, "push") != 0)
        return GHP_IGNORE_ACTION;

    // Repos permitidos hardcoded
    if (!ev->repo[0] || strcmp(ev->repo, "sk/ghd") != 0)
        return GHP_IGNORE_REPO;

    // Branches permitidas hardcoded
    if (!ev->ref[0] || strcmp(ev->ref, "refs/heads/main") != 0)
        return GHP_IGNORE_BRANCH;

    // Caminho local hardcoded
    snprintf(ev->exec_dir, sizeof(ev->exec_dir), "/home/sk/repos/ghd");

    return GHP_OK;
}

void github_log_event(const webhook_event_t *ev) {
    if (ev->action[0])
        printf("[ghd] action = %s\n", ev->action);
    if (ev->repo[0])
        printf("[ghd] repo   = %s\n", ev->repo);
    if (ev->ref[0])
        printf("[ghd] ref    = %s\n", ev->ref);
    if (ev->exec_dir[0])
        printf("[ghd] exec_dir = %s\n", ev->exec_dir);
}
