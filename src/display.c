#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>
#include "game.h"
#include "mural.h"

// Inicializar ncurses
void inicializar_ncurses() {
    initscr();              // Inicializar tela
    start_color();          // Habilitar cores
    raw();                  // Não precisar de Enter
    noecho();               // Não mostrar teclas digitadas
    keypad(stdscr, TRUE);   // Habilitar teclas especiais
    nodelay(stdscr, TRUE);  // getch() não-bloqueante
    curs_set(0);            // Esconder cursor
    
    // Definir pares de cores
    init_pair(1, COLOR_CYAN, COLOR_BLACK);    // Cabeçalho
    init_pair(2, COLOR_GREEN, COLOR_BLACK);   // Disponível
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);  // Trabalhando
    init_pair(4, COLOR_RED, COLOR_BLACK);     // Ocupado
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK); // Pedidos
    init_pair(6, COLOR_WHITE, COLOR_BLUE);    // Destaque
    init_pair(7, COLOR_BLACK, COLOR_WHITE);   // Contraste
}

// Finalizar ncurses
void finalizar_ncurses() {
    keypad(stdscr, FALSE);
    noraw();
    echo();
    curs_set(1);
    endwin();
}

// Desenhar borda com título
void desenhar_borda(int y, int x, int height, int width, const char* titulo) {
    // Desenhar borda
    for (int i = 0; i < height; i++) {
        mvaddch(y + i, x, '|');
        mvaddch(y + i, x + width - 1, '|');
    }
    for (int i = 0; i < width; i++) {
        mvaddch(y, x + i, '-');
        mvaddch(y + height - 1, x + i, '-');
    }
    
    // Cantos
    mvaddch(y, x, '+');
    mvaddch(y, x + width - 1, '+');
    mvaddch(y + height - 1, x, '+');
    mvaddch(y + height - 1, x + width - 1, '+');
    
    // Título
    if (titulo) {
        int titulo_len = strlen(titulo);
        int titulo_x = x + (width - titulo_len) / 2;
        attron(COLOR_PAIR(6));
        mvprintw(y, titulo_x, "%s", titulo);
        attroff(COLOR_PAIR(6));
    }
}

// Thread de exibição com ncurses
void* thread_exibir_info(void* arg) {
    EstadoJogo* estado = (EstadoJogo*)arg;
    
    // Aguardar inicialização
    sleep(2);
    
    // Inicializar ncurses
    inicializar_ncurses();
    
    while (estado->jogo_ativo) {
        // Limpar tela
        clear();
        
        // Obter dimensões da tela
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        
        // Cabeçalho principal
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(0, (max_x - 30) / 2, "🚀 FORA NO ESPAÇO 🚀");
        attroff(COLOR_PAIR(1) | A_BOLD);
        
        // Informações gerais (linha 2)
        attron(COLOR_PAIR(7));
        mvprintw(1, 2, " Tempo: %3ds | Completos: %2d | Tripulantes: %d ", 
                 estado->tempo_restante, estado->pedidos_completados, estado->num_tripulantes);
        attroff(COLOR_PAIR(7));
        
        // === SEÇÃO DE PEDIDOS (esquerda) ===
        int pedidos_x = 2;
        int pedidos_y = 3;
        int pedidos_width = max_x / 2 - 3;
        int pedidos_height = max_y - 6;
        
        desenhar_borda(pedidos_y, pedidos_x, pedidos_height, pedidos_width, " PEDIDOS PENDENTES ");
        
        // Mostrar pedidos
        pthread_mutex_lock(&estado->pedidos->mutex);
        int num_pedidos = estado->pedidos->count;
        mvprintw(pedidos_y + 1, pedidos_x + 2, "Total: %d pedidos", num_pedidos);
        
        Pedido* atual = estado->pedidos->primeiro;
        int linha = pedidos_y + 3;
        int count = 0;
        int max_pedidos = pedidos_height - 5;
        
        while (atual != NULL && count < max_pedidos) {
            time_t agora = time(NULL);
            int tempo_espera = (int)(agora - atual->criado_em);
            
            // Cor baseada no tempo de espera
            if (tempo_espera > 20) {
                attron(COLOR_PAIR(4)); // Vermelho para pedidos muito antigos
            } else if (tempo_espera > 10) {
                attron(COLOR_PAIR(3)); // Amarelo para pedidos antigos
            } else {
                attron(COLOR_PAIR(5)); // Magenta para pedidos novos
            }
            
            mvprintw(linha, pedidos_x + 2, "#%-2d %-10s %2ds", 
                     atual->id, NOMES_PRATOS[atual->tipo], tempo_espera);
            
            attroff(COLOR_PAIR(4));
            attroff(COLOR_PAIR(3));
            attroff(COLOR_PAIR(5));
            
            atual = atual->proximo;
            linha++;
            count++;
        }
        
        if (num_pedidos > max_pedidos) {
            mvprintw(linha, pedidos_x + 2, "... +%d pedidos", num_pedidos - max_pedidos);
        }
        
        pthread_mutex_unlock(&estado->pedidos->mutex);
        
        // === SEÇÃO DE STATUS (direita) ===
        int status_x = max_x / 2 + 1;
        int status_y = 3;
        int status_width = max_x / 2 - 3;
        int status_height = max_y - 6;
        
        desenhar_borda(status_y, status_x, status_height, status_width, " STATUS DA NAVE ");
        
        // Tripulantes
        mvprintw(status_y + 2, status_x + 2, "TRIPULANTES:");
        for (int i = 0; i < estado->num_tripulantes; i++) {
            pthread_mutex_lock(&estado->tripulantes[i].mutex);
            
            int cor_tripulante;
            const char* status_str;
            switch (estado->tripulantes[i].status) {
                case DISPONIVEL: 
                    cor_tripulante = 2; 
                    status_str = "DISPONIVEL"; 
                    break;
                case PREPARANDO_INGREDIENTES: 
                    cor_tripulante = 3; 
                    status_str = "PREP.INGRED"; 
                    break;
                case COZINHANDO: 
                    cor_tripulante = 3; 
                    status_str = "COZINHANDO"; 
                    break;
                case OCUPADO: 
                    cor_tripulante = 4; 
                    status_str = "OCUPADO"; 
                    break;
                default: 
                    cor_tripulante = 4; 
                    status_str = "DESCONHECIDO"; 
                    break;
            }
            
            attron(COLOR_PAIR(cor_tripulante));
            mvprintw(status_y + 3 + i, status_x + 4, "T%d: %-12s", i, status_str);
            attroff(COLOR_PAIR(cor_tripulante));
            
            if (estado->tripulantes[i].pedido_atual != NULL) {
                mvprintw(status_y + 3 + i, status_x + 18, "(#%d)", 
                         estado->tripulantes[i].pedido_atual->id);
            }
            
            pthread_mutex_unlock(&estado->tripulantes[i].mutex);
        }
        
        // Bancadas
        int recursos_y = status_y + 5 + estado->num_tripulantes;
        mvprintw(recursos_y, status_x + 2, "BANCADAS:");
        for (int i = 0; i < estado->num_tripulantes; i++) {
            pthread_mutex_lock(&estado->bancadas[i].mutex);
            
            if (estado->bancadas[i].status == LIVRE) {
                attron(COLOR_PAIR(2));
                mvprintw(recursos_y + 1, status_x + 4 + i * 8, "B%d:LIVRE", i);
            } else {
                attron(COLOR_PAIR(4));
                mvprintw(recursos_y + 1, status_x + 4 + i * 8, "B%d:T%d  ", i, 
                         estado->bancadas[i].ocupado_por_tripulante);
            }
            
            attroff(COLOR_PAIR(2));
            attroff(COLOR_PAIR(4));
            pthread_mutex_unlock(&estado->bancadas[i].mutex);
        }
        
        // Cozinhas
        mvprintw(recursos_y + 2, status_x + 2, "COZINHAS:");
        for (int i = 0; i < estado->num_tripulantes; i++) {
            pthread_mutex_lock(&estado->cozinhas[i].mutex);
            
            if (estado->cozinhas[i].status == LIVRE) {
                attron(COLOR_PAIR(2));
                mvprintw(recursos_y + 3, status_x + 4 + i * 8, "C%d:LIVRE", i);
            } else {
                attron(COLOR_PAIR(4));
                mvprintw(recursos_y + 3, status_x + 4 + i * 8, "C%d:T%d  ", i, 
                         estado->cozinhas[i].ocupado_por_tripulante);
            }
            
            attroff(COLOR_PAIR(2));
            attroff(COLOR_PAIR(4));
            pthread_mutex_unlock(&estado->cozinhas[i].mutex);
        }
        
        // Instruções na parte inferior
        attron(COLOR_PAIR(6));
        mvprintw(max_y - 2, 2, "Comandos: [q]Sair | [1-3][h|s|p|c]Atribuir pedido | Ctrl+C para emergência");
        attroff(COLOR_PAIR(6));
        
        // Atualizar tela
        refresh();
        
        // Aguardar um pouco antes da próxima atualização
        usleep(500000); // 0.5 segundos
    }
    
    // Finalizar ncurses
    finalizar_ncurses();
    
    return NULL;
}