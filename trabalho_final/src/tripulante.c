#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "game.h"

// Alocar uma bancada livre para o tripulante
int alocar_bancada(EstadoJogo* estado, int tripulante_id) {
    for (int i = 0; i < estado->num_tripulantes; i++) {
        pthread_mutex_lock(&estado->bancadas[i].mutex);
        
        if (estado->bancadas[i].status == LIVRE) {
            estado->bancadas[i].status = OCUPADO_POR;
            estado->bancadas[i].ocupado_por_tripulante = tripulante_id;
            pthread_mutex_unlock(&estado->bancadas[i].mutex);
            return i; // Retorna o ID da bancada alocada
        }
        
        pthread_mutex_unlock(&estado->bancadas[i].mutex);
    }
    
    return -1; // Nenhuma bancada disponível
}

// Alocar uma cozinha livre para o tripulante
int alocar_cozinha(EstadoJogo* estado, int tripulante_id) {
    for (int i = 0; i < estado->num_tripulantes; i++) {
        pthread_mutex_lock(&estado->cozinhas[i].mutex);
        
        if (estado->cozinhas[i].status == LIVRE) {
            estado->cozinhas[i].status = OCUPADO_POR;
            estado->cozinhas[i].ocupado_por_tripulante = tripulante_id;
            pthread_mutex_unlock(&estado->cozinhas[i].mutex);
            return i; // Retorna o ID da cozinha alocada
        }
        
        pthread_mutex_unlock(&estado->cozinhas[i].mutex);
    }
    
    return -1; // Nenhuma cozinha disponível
}

// Liberar bancada
void liberar_bancada(EstadoJogo* estado, int bancada_id) {
    if (bancada_id >= 0 && bancada_id < estado->num_tripulantes) {
        pthread_mutex_lock(&estado->bancadas[bancada_id].mutex);
        estado->bancadas[bancada_id].status = LIVRE;
        estado->bancadas[bancada_id].ocupado_por_tripulante = -1;
        pthread_mutex_unlock(&estado->bancadas[bancada_id].mutex);
    }
}

// Liberar cozinha
void liberar_cozinha(EstadoJogo* estado, int cozinha_id) {
    if (cozinha_id >= 0 && cozinha_id < estado->num_tripulantes) {
        pthread_mutex_lock(&estado->cozinhas[cozinha_id].mutex);
        estado->cozinhas[cozinha_id].status = LIVRE;
        estado->cozinhas[cozinha_id].ocupado_por_tripulante = -1;
        pthread_mutex_unlock(&estado->cozinhas[cozinha_id].mutex);
    }
}

// Atribuir pedido a um tripulante
bool atribuir_pedido_tripulante(EstadoJogo* estado, int tripulante_id, int pedido_id) {
    if (tripulante_id < 0 || tripulante_id >= estado->num_tripulantes) {
        return false;
    }
    
    Tripulante* tripulante = &estado->tripulantes[tripulante_id];
    
    pthread_mutex_lock(&tripulante->mutex);
    
    // Verificar se tripulante está disponível
    if (tripulante->status != DISPONIVEL) {
        pthread_mutex_unlock(&tripulante->mutex);
        return false;
    }
    
    // Remover pedido da lista
    Pedido* pedido = remover_pedido(estado->pedidos, pedido_id);
    if (pedido == NULL) {
        pthread_mutex_unlock(&tripulante->mutex);
        return false;
    }
    
    // Tentar alocar bancada
    int bancada_id = alocar_bancada(estado, tripulante_id);
    if (bancada_id == -1) {
        // Devolver pedido à lista se não conseguir bancada
        adicionar_pedido(estado->pedidos, pedido);
        pthread_mutex_unlock(&tripulante->mutex);
        return false;
    }
    
    // Atribuir pedido ao tripulante
    tripulante->pedido_atual = pedido;
    tripulante->bancada_atual = bancada_id;
    tripulante->status = PREPARANDO_INGREDIENTES;
    
    pthread_mutex_unlock(&tripulante->mutex);
    
    log_debug("[CHEF] Pedido #%d (%s) atribuído ao Tripulante %d na Bancada %d", 
           pedido_id, NOMES_PRATOS[pedido->tipo], tripulante_id, bancada_id);
    
    return true;
}

// Thread principal do tripulante
void* thread_tripulante(void* arg) {
    Tripulante* tripulante = (Tripulante*)arg;
    EstadoJogo* estado = NULL;
    
    // Encontrar o estado global (um pouco hacky, mas funciona para este projeto)
    // Em um projeto real, passaríamos uma estrutura com ambos
    extern EstadoJogo* estado_global;
    estado = estado_global;
    
    printf("[TRIPULANTE %d] Thread iniciada\n", tripulante->id);
    
    while (estado->jogo_ativo) {
        pthread_mutex_lock(&tripulante->mutex);
        
        if (tripulante->status == PREPARANDO_INGREDIENTES && tripulante->pedido_atual != NULL) {
            Pedido* pedido = tripulante->pedido_atual;
            int tempo_preparo = pedido->tempo_ingredientes;
            
            log_debug("[TRIPULANTE %d] Preparando ingredientes do pedido #%d (%s) por %ds", 
                   tripulante->id, pedido->id, NOMES_PRATOS[pedido->tipo], tempo_preparo);
            
            pthread_mutex_unlock(&tripulante->mutex);
            
            // Simular tempo de preparo dos ingredientes
            sleep(tempo_preparo);
            
            pthread_mutex_lock(&tripulante->mutex);
            
            // Liberar bancada
            liberar_bancada(estado, tripulante->bancada_atual);
            tripulante->bancada_atual = -1;
            
            // Tentar alocar cozinha
            int cozinha_id = alocar_cozinha(estado, tripulante->id);
            if (cozinha_id != -1) {
                tripulante->cozinha_atual = cozinha_id;
                tripulante->status = COZINHANDO;
                
                log_debug("[TRIPULANTE %d] Ingredientes prontos! Movendo para Cozinha %d", 
                       tripulante->id, cozinha_id);
            } else {
                // Se não há cozinha disponível, ficar esperando
                tripulante->status = OCUPADO;
                log_debug("[TRIPULANTE %d] Aguardando cozinha disponível...", tripulante->id);
            }
            
            pthread_mutex_unlock(&tripulante->mutex);
            
        } else if (tripulante->status == COZINHANDO && tripulante->pedido_atual != NULL) {
            Pedido* pedido = tripulante->pedido_atual;
            int tempo_cozinha = pedido->tempo_cozinha;
            
            log_debug("[TRIPULANTE %d] Cozinhando pedido #%d (%s) por %ds", 
                   tripulante->id, pedido->id, NOMES_PRATOS[pedido->tipo], tempo_cozinha);
            
            pthread_mutex_unlock(&tripulante->mutex);
            
            // Simular tempo de cozinha
            sleep(tempo_cozinha);
            
            pthread_mutex_lock(&tripulante->mutex);
            
            // Finalizar pedido
            log_debug("[TRIPULANTE %d] Pedido #%d (%s) COMPLETADO!", 
                   tripulante->id, pedido->id, NOMES_PRATOS[pedido->tipo]);
            
            // Liberar recursos
            liberar_cozinha(estado, tripulante->cozinha_atual);
            tripulante->cozinha_atual = -1;
            
            // Liberar pedido
            free(pedido);
            tripulante->pedido_atual = NULL;
            
            // Voltar ao status disponível
            tripulante->status = DISPONIVEL;
            
            // Incrementar contador de pedidos completados
            pthread_mutex_lock(&estado->mutex_global);
            estado->pedidos_completados++;
            
            // Verificar se atingiu a meta
            if (estado->pedidos_completados >= estado->meta_pedidos) {
                log_debug("[TRIPULANTE %d] META ATINGIDA! %d/%d pedidos completados!", 
                       tripulante->id, estado->pedidos_completados, estado->meta_pedidos);
                estado->jogo_ativo = false;
            }
            
            pthread_mutex_unlock(&estado->mutex_global);
            
            pthread_mutex_unlock(&tripulante->mutex);
            
        } else if (tripulante->status == OCUPADO && tripulante->pedido_atual != NULL) {
            // Tentar novamente alocar cozinha
            pthread_mutex_unlock(&tripulante->mutex);
            
            int cozinha_id = alocar_cozinha(estado, tripulante->id);
            if (cozinha_id != -1) {
                pthread_mutex_lock(&tripulante->mutex);
                tripulante->cozinha_atual = cozinha_id;
                tripulante->status = COZINHANDO;
                log_debug("[TRIPULANTE %d] Cozinha %d disponível! Iniciando cozimento", 
                       tripulante->id, cozinha_id);
                pthread_mutex_unlock(&tripulante->mutex);
            } else {
                // Aguardar um pouco antes de tentar novamente
                sleep(1);
            }
            
        } else {
            // Tripulante disponível, aguardar trabalho
            pthread_mutex_unlock(&tripulante->mutex);
            sleep(1);
        }
    }
    
    printf("[TRIPULANTE %d] Thread finalizada\n", tripulante->id);
    return NULL;
}