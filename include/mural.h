#ifndef MURAL_H
#define MURAL_H

#include "game.h"

// Funções da thread do mural
void* thread_mural_pedidos(void* arg);

// Funções auxiliares do mural
TipoPrato gerar_tipo_aleatorio(void);
int calcular_intervalo_pedidos(int num_tripulantes);
void imprimir_pedidos(ListaPedidos* lista);
Pedido* obter_primeiro_pedido(ListaPedidos* lista);
int contar_pedidos(ListaPedidos* lista);

#endif // MURAL_H