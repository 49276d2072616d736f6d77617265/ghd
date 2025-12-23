#include "include/exec.h"
#include "include/utils.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#define TIMEOUT_SEC 10

static void handle_timeout(int sig) {
    fprintf(stderr, "[ghd] child process timed out\n");
    _exit(1);
}

int exec_event(const webhook_event_t *ev)
{
    if (!ev->action[0] || !ev->repo[0] || !ev->exec_dir[0]) {
        fprintf(stderr, "[ghd] missing action, repo or exec_dir\n");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("[ghd] fork");
        return -1;
    }

    if (pid == 0) {
        // Child process
        signal(SIGALRM, handle_timeout);
        alarm(TIMEOUT_SEC);

        // PUSH
        if (strcmp(ev->action, "push") == 0) {
            printf("[ghd] push event for %s -> branch %s\n", ev->repo, ev->ref);
            execl("/usr/bin/git", "git", "-C", ev->exec_dir, "pull", NULL);
            perror("[ghd] exec git pull failed");
        }

        // PULL REQUEST
        else if (strcmp(ev->action, "pull_request") == 0) {
            const char *pr_branch = ev->ref;
            int pr_number = atoi(pr_branch + 3); // "pr/<n>"

            char local_branch[64];
            snprintf(local_branch, sizeof(local_branch), "pr/%d", pr_number);

            char fetch_ref[128];
            snprintf(fetch_ref, sizeof(fetch_ref), "refs/pull/%d/head:%s", pr_number, local_branch);

            printf("[ghd] pull_request for %s -> local branch %s (fetch ref=%s)\n",
                   ev->repo, local_branch, fetch_ref);

            if (fork() == 0) {
                execl("/usr/bin/git", "git", "-C", ev->exec_dir,
                      "fetch", "origin", fetch_ref, NULL);
                perror("[ghd] exec git fetch PR failed");
                _exit(1);
            }
            int status;
            wait(&status);

            if (fork() == 0) {
                execl("/usr/bin/git", "git", "-C", ev->exec_dir,
                      "checkout", "-B", local_branch, local_branch, NULL);
                perror("[ghd] exec git checkout PR failed");
                _exit(1);
            }
            wait(&status);

            // Log do título da PR
            char api_url[512];
            snprintf(api_url, sizeof(api_url),
                     "https://api.github.com/repos/%s/pulls/%d", ev->repo, pr_number);

            char cmd[1024];
            snprintf(cmd, sizeof(cmd),
                     "curl -s %s | grep -m1 '\"title\"' | sed 's/.*: \"\\(.*\\)\",/\\1/'", api_url);
            FILE *fp = popen(cmd, "r");
            if (fp) {
                char pr_title[256];
                if (fgets(pr_title, sizeof(pr_title), fp)) {
                    pr_title[strcspn(pr_title, "\n")] = 0;
                    printf("[ghd] PR #%d title: %s\n", pr_number, pr_title);
                }
                pclose(fp);
            }
        }

        // ISSUES
        else if (strcmp(ev->action, "issues") == 0) {
            printf("[ghd] issues event for %s (ref=%s)\n", ev->repo, ev->ref);

            int issue_number = atoi(ev->ref + 6); // "issue/<n>"

            char api_url[512];
            snprintf(api_url, sizeof(api_url),
                     "https://api.github.com/repos/%s/issues/%d",
                     ev->repo, issue_number);

            // Verifica se o issue existe
            char curl_code[1024];
            snprintf(curl_code, sizeof(curl_code),
                     "curl -s -o /dev/null -w \"%%{http_code}\" %s", api_url);
            FILE *fp = popen(curl_code, "r");
            if (!fp) _exit(1);

            char http_code[16];
            if (!fgets(http_code, sizeof(http_code), fp)) { pclose(fp); _exit(1); }
            pclose(fp);

            if (atoi(http_code) != 200) {
                printf("[ghd] issue #%d does not exist (HTTP %s), skipping log\n",
                       issue_number, http_code);
            } else {
                char log_dir[256];
                snprintf(log_dir, sizeof(log_dir), "%s/issues_logs", ev->exec_dir);
                mkdir(log_dir, 0755);

                char log_file[512];
                snprintf(log_file, sizeof(log_file), "%s/issue_%d.txt", log_dir, issue_number);

                // Pega título e body do issue
                char cmd[1024];
                snprintf(cmd, sizeof(cmd),
                "curl -s %s | sed -n "
                "'s/.*\"title\": \"\\([^\"]*\\)\".*/Title: \\1/p; "
                "s/.*\"body\": \"\\([^\"]*\\)\".*/Body: \\1/p'", api_url);


                FILE *fdata = popen(cmd, "r");
                if (fdata) {
                    FILE *f = fopen(log_file, "w");
                    if (f) {
                        char line[1024];
                        fprintf(f, "Issue #%d triggered on repo %s\n", issue_number, ev->repo);
                        while (fgets(line, sizeof(line), fdata)) {
                            fputs(line, f);
                        }
                        fclose(f);
                        printf("[ghd] issue #%d logged to %s\n", issue_number, log_file);
                    } else {
                        fprintf(stderr, "[ghd] failed to create issue log %s\n", log_file);
                    }
                    pclose(fdata);
                }
            }
        }

        else {
            printf("[ghd] unknown action: %s\n", ev->action);
        }

        _exit(0);
    }

    // Parent process
    int status;
    pid_t w = waitpid(pid, &status, 0);
    if (w < 0) { perror("[ghd] waitpid"); return -1; }

    if (WIFEXITED(status)) {
        int ret = WEXITSTATUS(status);
        printf("[ghd] exec finished with code %d\n", ret);
        return ret;
    } else if (WIFSIGNALED(status)) {
        printf("[ghd] exec killed by signal %d\n", WTERMSIG(status));
        return -1;
    } else {
        printf("[ghd] exec failed\n");
        return -1;
    }
}
