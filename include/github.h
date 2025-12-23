#pragma once
#include "json.h"

/*
 * Decide se o evento deve ser processado.
 * Retorna 1 = processar
 * Retorna 0 = ignorar silenciosamente
 */
int github_should_process(const webhook_event_t *ev);

/*
 * Log básico / debug do evento
 */
void github_log_event(const webhook_event_t *ev);
