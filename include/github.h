#pragma once
#include "json.h"

int github_should_process(const webhook_event_t *ev);

void github_log_event(const webhook_event_t *ev);
