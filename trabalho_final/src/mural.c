#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "game.h"
#include "utils.h"

// Gerar um tipo de prato aleatório
TipoPrato gerar_tipo_aleatorio() {
    return (TipoPrato)(rand() % NUM_TIPOS_PRATO);
}

// Calcular intervalo entre pedidos baseado na dificuldade
int calcular_intervalo_pedidos(int num_tripulantes) {
    // Mais tripulantes = pedidos mais frequentes
    switch (num_tripulantes) {
        case 1: return 8;  // 8 segundos entre pedidos
        case 2: return 6;  // 6 segundos entre pedidos  
        case 3: return 4;  // 4 segundos entre pedidos
        default: return 8;
    }
}

// Thread responsável por gerar pedidos automaticamente
void* thread_mural_pedidos(void* arg) {
    EstadoJogo* estado = (EstadoJogo*)arg;
    
    log_debug("[MURAL] Thread do mural iniciada");
    
    int intervalo = calcular_intervalo_pedidos(estado->num_tripulantes);
    log_debug("[MURAL] Gerando pedidos a cada %d segundos", intervalo);
    
    // Aguardar um pouco antes de começar a gerar pedidos
    sleep(3);
    
    while (estado->jogo_ativo) {
        // Verificar se não há muitos pedidos acumulados
        pthread_mutex_lock(&estado->pedidos->mutex);
        int num_pedidos = estado->pedidos->count;
        pthread_mutex_unlock(&estado->pedidos->mutex);
        
        if (num_pedidos < MAX_PEDIDOS) {
            // Gerar novo pedido
            TipoPrato tipo = gerar_tipo_aleatorio();
            Pedido* novo_pedido = criar_pedido(tipo);
            
            if (novo_pedido != NULL) {
                adicionar_pedido(estado->pedidos, novo_pedido);
                
                log_debug("[MURAL] Novo pedido criado: #%d - %s (Ingredientes: %ds, Cozinha: %ds)", 
                       novo_pedido->id, 
                       NOMES_PRATOS[tipo],
                       novo_pedido->tempo_ingredientes,
                       novo_pedido->tempo_cozinha);
            } else {
                log_debug("[MURAL] ERRO: Falha ao criar pedido");
            }
        } else {
            log_debug("[MURAL] Muitos pedidos acumulados (%d), aguardando...", num_pedidos);
        }
        
        // Aguardar antes do próximo pedido
        sleep(intervalo);
    }
    
    log_debug("[MURAL] Thread do mural finalizada");
    return NULL;
}

// Função para imprimir status da lista de pedidos (para debug)
void imprimir_pedidos(ListaPedidos* lista) {
    if (lista == NULL) return;
    
    pthread_mutex_lock(&lista->mutex);
    
    printf("[MURAL] === LISTA DE PEDIDOS (%d) ===\n", lista->count);
    
    Pedido* atual = lista->primeiro;
    int pos = 1;
    
    while (atual != NULL) {
        time_t agora = time(NULL);
        int tempo_espera = (int)(agora - atual->criado_em);
        
        printf("[MURAL] %d. Pedido #%d: %s (Esperando há %ds)\n", 
               pos++, atual->id, NOMES_PRATOS[atual->tipo], tempo_espera);
        atual = atual->proximo;
    }
    
    if (lista->count == 0) {
        printf("[MURAL] Nenhum pedido pendente\n");
    }
    
    printf("[MURAL] ================================\n");
    
    pthread_mutex_unlock(&lista->mutex);
}

// Função para obter o primeiro pedido da lista (sem remover)
Pedido* obter_primeiro_pedido(ListaPedidos* lista) {
    if (lista == NULL) return NULL;
    
    pthread_mutex_lock(&lista->mutex);
    Pedido* primeiro = lista->primeiro;
    pthread_mutex_unlock(&lista->mutex);
    
    return primeiro;
}

// Função para contar pedidos na lista
int contar_pedidos(ListaPedidos* lista) {
    if (lista == NULL) return 0;
    
    pthread_mutex_lock(&lista->mutex);
    int count = lista->count;
    pthread_mutex_unlock(&lista->mutex);
    
    return count;
}