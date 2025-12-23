#pragma once
#include "json.h"

typedef enum {
    GHP_OK = 1,               // Evento válido e processável
    GHP_IGNORE_ACTION = -1,   // Ação que não nos interessa
    GHP_IGNORE_REPO = -2,     // Repositório não configurado
    GHP_IGNORE_BRANCH = -3,   // Branch não monitorada
    GHP_IGNORE_USER = -4,     // Usuário que não queremos processar
    GHP_IGNORE_LABEL = -5,    // Label que não nos interessa
    GHP_IGNORE_EVENT_TYPE = -6, // Tipo de evento não tratado (ex: pull_request_review)
    GHP_ERROR_PARSE = -7,     // Erro ao parsear JSON
    GHP_ERROR_HMAC = -8       // HMAC inválido
} ghp_status_t;


int github_should_process(webhook_event_t *ev);

void github_log_event(const webhook_event_t *ev);
