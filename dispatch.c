#include "include/dispatch.h"
#include "include/exec.h"
#include <stdio.h>

int dispatch_event(const webhook_event_t *ev)
{
    printf("[ghd] dispatching event...\n");

    int ret = exec_event(ev);

    printf("[ghd] dispatch finished\n");
    return ret;
}
