#ifndef GAME_H
#define GAME_H

#include <pthread.h>
#include <ncurses.h>
#include <stdbool.h>

// Configurações do jogo
#define MAX_TRIPULANTES 3
#define MAX_BANCADAS 3
#define MAX_COZINHAS 3
#define MAX_PEDIDOS 10
#define TEMPO_JOGO 600  // segundos

// Tipos de pratos
typedef enum {
    HAMBURGUER,
    SUCO,
    PIZZA,
    CAFE,
    NUM_TIPOS_PRATO
} TipoPrato;

// Status dos tripulantes
typedef enum {
    DISPONIVEL,
    PREPARANDO_INGREDIENTES,
    COZINHANDO,
    OCUPADO
} StatusTripulante;

// Status dos recursos
typedef enum {
    LIVRE,
    OCUPADO_POR
} StatusRecurso;

// Estrutura de um pedido
typedef struct pedido {
    int id;
    TipoPrato tipo;
    int tempo_ingredientes;  // em segundos
    int tempo_cozinha;      // em segundos
    time_t criado_em;
    struct pedido* proximo;
} Pedido;

// Lista de pedidos
typedef struct {
    Pedido* primeiro;
    Pedido* ultimo;
    int count;
    pthread_mutex_t mutex;
} ListaPedidos;

// Estrutura de um tripulante
typedef struct {
    int id;
    StatusTripulante status;
    Pedido* pedido_atual;
    int bancada_atual;
    int cozinha_atual;
    pthread_t thread;
    pthread_mutex_t mutex;
} Tripulante;

// Estrutura de bancada/cozinha
typedef struct {
    int id;
    StatusRecurso status;
    int ocupado_por_tripulante;  // -1 se livre
    pthread_mutex_t mutex;
} Recurso;

// Estado global do jogo
typedef struct {
    // Recursos
    Recurso bancadas[MAX_BANCADAS];
    Recurso cozinhas[MAX_COZINHAS];
    
    // Tripulantes
    Tripulante tripulantes[MAX_TRIPULANTES];
    int num_tripulantes;
    
    // Pedidos
    ListaPedidos* pedidos;
    
    // Controle do jogo
    bool jogo_ativo;
    int tempo_restante;
    int pedidos_completados;
    
    // Threads
    pthread_t thread_mural;
    pthread_t thread_chef;
    pthread_t thread_display;
    
    // Mutex global
    pthread_mutex_t mutex_global;
    
} EstadoJogo;

// Tempos de preparo por tipo de prato
extern const int TEMPOS_INGREDIENTES[NUM_TIPOS_PRATO];
extern const int TEMPOS_COZINHA[NUM_TIPOS_PRATO];
extern const char* NOMES_PRATOS[NUM_TIPOS_PRATO];

// Funções principais
void inicializar_jogo(EstadoJogo* estado, int num_tripulantes);
void finalizar_jogo(EstadoJogo* estado);
void* thread_mural_pedidos(void* arg);
void* thread_chef_cozinha(void* arg);
void* thread_tripulante(void* arg);
void* thread_exibir_info(void* arg);

// Funções auxiliares
Pedido* criar_pedido(TipoPrato tipo);
void adicionar_pedido(ListaPedidos* lista, Pedido* pedido);
Pedido* remover_pedido(ListaPedidos* lista, int id);
void destruir_lista_pedidos(ListaPedidos* lista);

int alocar_bancada(EstadoJogo* estado, int tripulante_id);
int alocar_cozinha(EstadoJogo* estado, int tripulante_id);
void liberar_bancada(EstadoJogo* estado, int bancada_id);
void liberar_cozinha(EstadoJogo* estado, int cozinha_id);

#endif // GAME_H
