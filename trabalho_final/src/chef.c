#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "game.h"
#include "mural.h"
#include "tripulante.h"

// Obter pedido por tipo de prato
Pedido* obter_pedido_por_tipo(ListaPedidos* lista, TipoPrato tipo) {
    if (lista == NULL) return NULL;
    
    pthread_mutex_lock(&lista->mutex);
    
    Pedido* atual = lista->primeiro;
    while (atual != NULL) {
        if (atual->tipo == tipo) {
            pthread_mutex_unlock(&lista->mutex);
            return atual;
        }
        atual = atual->proximo;
    }
    
    pthread_mutex_unlock(&lista->mutex);
    return NULL;
}

// Obter primeiro pedido da lista
Pedido* obter_primeiro_pedido_disponivel(ListaPedidos* lista) {
    if (lista == NULL) return NULL;
    
    pthread_mutex_lock(&lista->mutex);
    Pedido* primeiro = lista->primeiro;
    pthread_mutex_unlock(&lista->mutex);
    
    return primeiro;
}

// Processar comando do usuário
bool processar_comando(EstadoJogo* estado, int tripulante_id, char tipo_prato) {
    if (tripulante_id < 0 || tripulante_id >= estado->num_tripulantes) {
        return false;
    }
    
    TipoPrato tipo;
    const char* nome_comando;
    
    // Mapear caractere para tipo de prato
    switch (tipo_prato) {
        case 'h': case 'H': 
            tipo = HAMBURGUER; 
            nome_comando = "Hamburguer";
            break;
        case 's': case 'S': 
            tipo = SUCO; 
            nome_comando = "Suco";
            break;
        case 'p': case 'P': 
            tipo = PIZZA; 
            nome_comando = "Pizza";
            break;
        case 'c': case 'C': 
            tipo = CAFE; 
            nome_comando = "Cafe";
            break;
        default: 
            return false;
    }
    
    // Primeiro, tentar encontrar um pedido específico do tipo solicitado
    Pedido* pedido_especifico = obter_pedido_por_tipo(estado->pedidos, tipo);
    
    if (pedido_especifico != NULL) {
        // Atribuir pedido específico
        bool sucesso = atribuir_pedido_tripulante(estado, tripulante_id, pedido_especifico->id);
        if (sucesso) {
            log_debug("[CHEF] Comando: T%d fazer %s - Pedido específico #%d atribuído", 
                   tripulante_id, nome_comando, pedido_especifico->id);
            return true;
        }
    }
    
    // Se não há pedido específico ou falhou, tentar atribuir o primeiro pedido disponível
    Pedido* primeiro = obter_primeiro_pedido_disponivel(estado->pedidos);
    if (primeiro != NULL) {
        bool sucesso = atribuir_pedido_tripulante(estado, tripulante_id, primeiro->id);
        if (sucesso) {
            log_debug("[CHEF] Comando: T%d fazer %s - Primeiro pedido #%d (%s) atribuído", 
                   tripulante_id, nome_comando, primeiro->id, NOMES_PRATOS[primeiro->tipo]);
            return true;
        }
    }
    
    return false;
}

// Thread do chef da cozinha (processamento de comandos)
void* thread_chef_cozinha(void* arg) {
    EstadoJogo* estado = (EstadoJogo*)arg;
    
    log_debug("[CHEF] Thread do chef iniciada");
    
    char ultimo_comando = 0;
    char comando_atual = 0;
    
    while (estado->jogo_ativo) {
        // Ler tecla sem bloquear (ncurses já configurado em modo não-bloqueante)
        int tecla = getch();
        
        if (tecla != ERR) {
            if (tecla == 'q' || tecla == 'Q') {
                log_debug("[CHEF] Comando de saída recebido");
                estado->jogo_ativo = false;
                break;
            }
            
            // Sistema de buffer de 2 comandos
            if (ultimo_comando == 0) {
                // Primeiro caractere do comando
                if (tecla >= '1' && tecla <= '3') {
                    ultimo_comando = tecla;
                    log_debug("[CHEF] Tripulante %c selecionado, aguardando tipo de prato...", tecla);
                }
            } else {
                // Segundo caractere do comando  
                comando_atual = tecla;
                
                // Processar comando completo
                int tripulante_id = ultimo_comando - '1'; // Converter '1'-'3' para 0-2
                
                if (tripulante_id < estado->num_tripulantes) {
                    bool sucesso = processar_comando(estado, tripulante_id, comando_atual);
                    
                    if (!sucesso) {
                        log_debug("[CHEF] Comando falhou: T%d não pode fazer '%c' agora", 
                               tripulante_id, comando_atual);
                        log_debug("[CHEF] Motivos possíveis: tripulante ocupado, sem pedidos, sem bancada");
                    }
                } else {
                    log_debug("[CHEF] Tripulante inválido: %d (máximo: %d)", 
                           tripulante_id, estado->num_tripulantes - 1);
                }
                
                // Resetar buffer de comandos
                ultimo_comando = 0;
                comando_atual = 0;
            }
        }
        
        // Pequena pausa para não consumir CPU desnecessariamente
        usleep(100000); // 0.1 segundos
    }
    
    log_debug("[CHEF] Thread do chef finalizada");
    return NULL;
}