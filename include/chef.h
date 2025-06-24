#ifndef CHEF_H
#define CHEF_H

#include "game.h"
#include <stdbool.h>

// Thread do chef
void* thread_chef_cozinha(void* arg);

// Funções de processamento de comandos
bool processar_comando(EstadoJogo* estado, int tripulante_id, char tipo_prato);
Pedido* obter_pedido_por_tipo(ListaPedidos* lista, TipoPrato tipo);
Pedido* obter_primeiro_pedido_disponivel(ListaPedidos* lista);

#endif // CHEF_H