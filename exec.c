#include "include/exec.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int exec_event(const webhook_event_t *ev)
{
    if (!ev->action[0] || !ev->repo[0] || !ev->exec_dir[0])
        return -1;

    pid_t pid = fork();
    if (pid < 0) {
        perror("[ghd] fork");
        return -1;
    }

    if (pid == 0) {
        if (strcmp(ev->action, "push") == 0) {
            execl("/usr/bin/git", "git", "-C", ev->exec_dir, "pull", NULL);
            perror("[ghd] exec git");
        }
        _exit(1);
    }

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        printf("[ghd] exec finished with code %d\n", WEXITSTATUS(status));
        return 0;
    } else {
        printf("[ghd] exec failed\n");
        return -1;
    }
}
