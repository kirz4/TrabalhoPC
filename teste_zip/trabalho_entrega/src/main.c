#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "game.h"
#include "mural.h"
#include "display.h"
#include "chef.h"
#include "tripulante.h"
#include "utils.h"

EstadoJogo* estado_global = NULL;
time_t tempo_inicio = 0;

// Thread para gerenciar o tempo do jogo
void* thread_tempo_jogo(void* arg) {
    EstadoJogo* estado = (EstadoJogo*)arg;
    tempo_inicio = time(NULL);
    
    while (estado->jogo_ativo) {
        time_t agora = time(NULL);
        int tempo_decorrido = (int)(agora - tempo_inicio);
        estado->tempo_restante = TEMPO_JOGO - tempo_decorrido;
        
        if (estado->tempo_restante <= 0) {
            estado->jogo_ativo = false;
            break;
        }
        
        // Verificar condições de derrota
        pthread_mutex_lock(&estado->pedidos->mutex);
        int pedidos_acumulados = estado->pedidos->count;
        pthread_mutex_unlock(&estado->pedidos->mutex);
        
        if (pedidos_acumulados >= MAX_PEDIDOS) {
            estado->jogo_ativo = false;
            break;
        }
        
        sleep(1);
    }
    
    return NULL;
}

// Handler para CTRL+C
void signal_handler(int sig) {
    (void)sig; // Evitar warning de parâmetro não usado
    if (estado_global != NULL) {
        estado_global->jogo_ativo = false;
        // Finalizar ncurses se estiver ativo
        endwin();
        printf("\nJogo interrompido pelo usuário.\n");
    }
}

int main(int argc, char* argv[]) {
    int num_tripulantes = 2; // Padrão
    
    // Processar argumentos de linha de comando
    if (argc > 1) {
        num_tripulantes = atoi(argv[1]);
        if (num_tripulantes < 1 || num_tripulantes > MAX_TRIPULANTES) {
            printf("Número de tripulantes deve estar entre 1 e %d\n", MAX_TRIPULANTES);
            return 1;
        }
    }
    
    // Configurar signal handler
    signal(SIGINT, signal_handler);
    
    // Inicializar random
    srand(time(NULL));
    
    // Inicializar sistema de log
    inicializar_log("jogo.log");
    
    // Alocar e inicializar estado do jogo
    EstadoJogo estado;
    estado_global = &estado;
    
    printf("=== FORA NO ESPAÇO ===\n");
    printf("Inicializando jogo com %d tripulante(s)...\n", num_tripulantes);
    
    // Inicializar jogo
    inicializar_jogo(&estado, num_tripulantes);
    
    // Criar threads
    printf("Criando threads...\n");
    
    // Thread do tempo
    // pthread_t thread_tempo;
    // if (pthread_create(&thread_tempo, NULL, thread_tempo_jogo, &estado) != 0) {
    //     fprintf(stderr, "Erro ao criar thread do tempo\n");
    //     finalizar_jogo(&estado);
    //     return 1;
    // }
    
    // Thread do mural de pedidos
    if (pthread_create(&estado.thread_mural, NULL, thread_mural_pedidos, &estado) != 0) {
        fprintf(stderr, "Erro ao criar thread do mural\n");
        finalizar_jogo(&estado);
        return 1;
    }
    
    // Thread do display
    if (pthread_create(&estado.thread_display, NULL, thread_exibir_info, &estado) != 0) {
        fprintf(stderr, "Erro ao criar thread de display\n");
        finalizar_jogo(&estado);
        return 1;
    }
    
    // Threads dos tripulantes
    for (int i = 0; i < estado.num_tripulantes; i++) {
        if (pthread_create(&estado.tripulantes[i].thread, NULL, thread_tripulante, &estado.tripulantes[i]) != 0) {
            fprintf(stderr, "Erro ao criar thread do tripulante %d\n", i);
            finalizar_jogo(&estado);
            return 1;
        }
    }
    
    printf("Todas as threads criadas. Iniciando o jogo...\n");
    sleep(2);
    
    // Thread principal vira o chef da cozinha
    thread_chef_cozinha(&estado);
    
    // Esperar threads terminarem
    printf("Finalizando threads...\n");
    
    // pthread_join(thread_tempo, NULL);
    pthread_join(estado.thread_mural, NULL);
    pthread_join(estado.thread_display, NULL);
    
    for (int i = 0; i < estado.num_tripulantes; i++) {
        pthread_join(estado.tripulantes[i].thread, NULL);
    }
    
    // Finalizar jogo
    finalizar_jogo(&estado);
    
    // Mostrar resultado final usando ncurses
    mostrar_tela_final(&estado);
    
    // Finalizar sistema de log
    finalizar_log();
    
    return 0;
}