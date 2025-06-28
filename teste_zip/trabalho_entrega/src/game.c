#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game.h"

// Tempos de preparo por tipo de prato (em segundos)
const int TEMPOS_INGREDIENTES[NUM_TIPOS_PRATO] = {5, 2, 7, 3}; // HAMBURGUER, SUCO, PIZZA, CAFE
const int TEMPOS_COZINHA[NUM_TIPOS_PRATO] = {8, 1, 12, 4};     // HAMBURGUER, SUCO, PIZZA, CAFE
const char* NOMES_PRATOS[NUM_TIPOS_PRATO] = {"Hamburguer", "Suco", "Pizza", "Cafe"};

// Inicializar o estado do jogo
void inicializar_jogo(EstadoJogo* estado, int num_tripulantes) {
    printf("Inicializando estado do jogo...\n");
    
    // Validar número de tripulantes
    if (num_tripulantes < 1 || num_tripulantes > MAX_TRIPULANTES) {
        fprintf(stderr, "Número inválido de tripulantes: %d\n", num_tripulantes);
        exit(1);
    }
    
    estado->num_tripulantes = num_tripulantes;
    estado->jogo_ativo = true;
    estado->tempo_restante = TEMPO_JOGO;
    estado->pedidos_completados = 0;
    
    // Inicializar mutex global
    if (pthread_mutex_init(&estado->mutex_global, NULL) != 0) {
        fprintf(stderr, "Erro ao inicializar mutex global\n");
        exit(1);
    }
    
    // Inicializar bancadas
    printf("Inicializando %d bancadas...\n", num_tripulantes);
    for (int i = 0; i < num_tripulantes; i++) {
        estado->bancadas[i].id = i;
        estado->bancadas[i].status = LIVRE;
        estado->bancadas[i].ocupado_por_tripulante = -1;
        
        if (pthread_mutex_init(&estado->bancadas[i].mutex, NULL) != 0) {
            fprintf(stderr, "Erro ao inicializar mutex da bancada %d\n", i);
            exit(1);
        }
    }
    
    // Inicializar cozinhas
    printf("Inicializando %d cozinhas...\n", num_tripulantes);
    for (int i = 0; i < num_tripulantes; i++) {
        estado->cozinhas[i].id = i;
        estado->cozinhas[i].status = LIVRE;
        estado->cozinhas[i].ocupado_por_tripulante = -1;
        
        if (pthread_mutex_init(&estado->cozinhas[i].mutex, NULL) != 0) {
            fprintf(stderr, "Erro ao inicializar mutex da cozinha %d\n", i);
            exit(1);
        }
    }
    
    // Inicializar tripulantes
    printf("Inicializando %d tripulantes...\n", num_tripulantes);
    for (int i = 0; i < num_tripulantes; i++) {
        estado->tripulantes[i].id = i;
        estado->tripulantes[i].status = DISPONIVEL;
        estado->tripulantes[i].pedido_atual = NULL;
        estado->tripulantes[i].bancada_atual = -1;
        estado->tripulantes[i].cozinha_atual = -1;
        
        if (pthread_mutex_init(&estado->tripulantes[i].mutex, NULL) != 0) {
            fprintf(stderr, "Erro ao inicializar mutex do tripulante %d\n", i);
            exit(1);
        }
    }
    
    // Inicializar lista de pedidos
    printf("Inicializando lista de pedidos...\n");
    estado->pedidos = malloc(sizeof(ListaPedidos));
    if (estado->pedidos == NULL) {
        fprintf(stderr, "Erro ao alocar memória para lista de pedidos\n");
        exit(1);
    }
    
    estado->pedidos->primeiro = NULL;
    estado->pedidos->ultimo = NULL;
    estado->pedidos->count = 0;
    
    if (pthread_mutex_init(&estado->pedidos->mutex, NULL) != 0) {
        fprintf(stderr, "Erro ao inicializar mutex da lista de pedidos\n");
        exit(1);
    }
    
    printf("Inicialização concluída!\n");
}

// Finalizar o jogo e limpar recursos
void finalizar_jogo(EstadoJogo* estado) {
    printf("Finalizando jogo...\n");
    
    estado->jogo_ativo = false;
    
    // Destruir mutex global
    pthread_mutex_destroy(&estado->mutex_global);
    
    // Destruir mutex das bancadas
    for (int i = 0; i < estado->num_tripulantes; i++) {
        pthread_mutex_destroy(&estado->bancadas[i].mutex);
    }
    
    // Destruir mutex das cozinhas
    for (int i = 0; i < estado->num_tripulantes; i++) {
        pthread_mutex_destroy(&estado->cozinhas[i].mutex);
    }
    
    // Destruir mutex dos tripulantes
    for (int i = 0; i < estado->num_tripulantes; i++) {
        pthread_mutex_destroy(&estado->tripulantes[i].mutex);
    }
    
    // Destruir lista de pedidos
    if (estado->pedidos != NULL) {
        destruir_lista_pedidos(estado->pedidos);
        free(estado->pedidos);
        estado->pedidos = NULL;
    }
    
    printf("Recursos liberados!\n");
}

// Criar um novo pedido
Pedido* criar_pedido(TipoPrato tipo) {
    static int proximo_id = 1;
    
    Pedido* pedido = malloc(sizeof(Pedido));
    if (pedido == NULL) {
        fprintf(stderr, "Erro ao alocar memória para pedido\n");
        return NULL;
    }
    
    pedido->id = proximo_id++;
    pedido->tipo = tipo;
    pedido->tempo_ingredientes = TEMPOS_INGREDIENTES[tipo];
    pedido->tempo_cozinha = TEMPOS_COZINHA[tipo];
    pedido->criado_em = time(NULL);
    pedido->proximo = NULL;
    
    return pedido;
}

// Adicionar pedido à lista
void adicionar_pedido(ListaPedidos* lista, Pedido* pedido) {
    if (lista == NULL || pedido == NULL) return;
    
    pthread_mutex_lock(&lista->mutex);
    
    if (lista->primeiro == NULL) {
        lista->primeiro = pedido;
        lista->ultimo = pedido;
    } else {
        lista->ultimo->proximo = pedido;
        lista->ultimo = pedido;
    }
    
    lista->count++;
    
    pthread_mutex_unlock(&lista->mutex);
}

// Remover pedido da lista por ID
Pedido* remover_pedido(ListaPedidos* lista, int id) {
    if (lista == NULL) return NULL;
    
    pthread_mutex_lock(&lista->mutex);
    
    Pedido* atual = lista->primeiro;
    Pedido* anterior = NULL;
    
    while (atual != NULL) {
        if (atual->id == id) {
            // Remover da lista
            if (anterior == NULL) {
                lista->primeiro = atual->proximo;
            } else {
                anterior->proximo = atual->proximo;
            }
            
            if (atual == lista->ultimo) {
                lista->ultimo = anterior;
            }
            
            lista->count--;
            atual->proximo = NULL;
            
            pthread_mutex_unlock(&lista->mutex);
            return atual;
        }
        
        anterior = atual;
        atual = atual->proximo;
    }
    
    pthread_mutex_unlock(&lista->mutex);
    return NULL;
}

// Destruir lista de pedidos
void destruir_lista_pedidos(ListaPedidos* lista) {
    if (lista == NULL) return;
    
    pthread_mutex_lock(&lista->mutex);
    
    Pedido* atual = lista->primeiro;
    while (atual != NULL) {
        Pedido* proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    
    lista->primeiro = NULL;
    lista->ultimo = NULL;
    lista->count = 0;
    
    pthread_mutex_unlock(&lista->mutex);
    pthread_mutex_destroy(&lista->mutex);
}