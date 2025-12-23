#include "include/github.h"
#include "include/utils.h"
#include <stdio.h>
#include <string.h>

ghp_status_t github_should_process(webhook_event_t *ev) {

    // aceitar apenas eventos que nos interessam
    if (!ev->action[0])
        return GHP_IGNORE_ACTION;

    if (strcmp(ev->action, "push") != 0 &&
        strcmp(ev->action, "pull_request") != 0 &&
        strcmp(ev->action, "issues") != 0)
    {
        return GHP_IGNORE_ACTION;
    }

    // aceitar qualquer repo (ou filtrar)
    if (!ev->repo[0])
        return GHP_IGNORE_REPO;

    // opcional: aceitar qualquer branch para push
    if (strcmp(ev->action, "push") == 0 &&
        (!ev->ref[0] || strcmp(ev->ref, "refs/heads/main") != 0))
    {
        return GHP_IGNORE_BRANCH;
    }

    // definir diretório de execução SOMENTE se estiver vazio
    if (!ev->exec_dir[0]) {
        snprintf(ev->exec_dir, sizeof(ev->exec_dir), "/home/user/repos/ghd");
    }

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
