#ifndef TRIPULANTE_H
#define TRIPULANTE_H

#include "game.h"
#include <stdbool.h>

// Thread principal do tripulante
void* thread_tripulante(void* arg);

// Funções de gerenciamento de recursos
int alocar_bancada(EstadoJogo* estado, int tripulante_id);
int alocar_cozinha(EstadoJogo* estado, int tripulante_id);
void liberar_bancada(EstadoJogo* estado, int bancada_id);
void liberar_cozinha(EstadoJogo* estado, int cozinha_id);

// Funções de atribuição de pedidos
bool atribuir_pedido_tripulante(EstadoJogo* estado, int tripulante_id, int pedido_id);

#endif // TRIPULANTE_H