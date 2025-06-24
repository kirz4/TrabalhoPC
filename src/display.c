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
        mvprintw(0, (max_x - 20) / 2, "*** FORA NO ESPACO ***");
        attroff(COLOR_PAIR(1) | A_BOLD);
        
        // Informações gerais (linha 2)
        int minutos = estado->tempo_restante / 60;
        int segundos = estado->tempo_restante % 60;
        
        // Cor baseada no tempo restante
        if (estado->tempo_restante <= 30) {
            attron(COLOR_PAIR(4)); // Vermelho se crítico
        } else if (estado->tempo_restante <= 60) {
            attron(COLOR_PAIR(3)); // Amarelo se baixo
        } else {
            attron(COLOR_PAIR(7)); // Branco normal
        }
        
        mvprintw(1, 2, " Tempo: %02d:%02d | Completos: %2d | Tripulantes: %d ", 
                 minutos, segundos, estado->pedidos_completados, estado->num_tripulantes);
        
        attroff(COLOR_PAIR(4));
        attroff(COLOR_PAIR(3));
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
        mvprintw(max_y - 2, 2, "Comandos: [q]Sair | [1-3][h|s|p|c]Atribuir pedido | Ctrl+C para emergencia");
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

// Função para mostrar tela final com ncurses
void mostrar_tela_final(EstadoJogo* estado) {
    // Inicializar ncurses para tela final
    inicializar_ncurses();
    
    clear();
    
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    // Título
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(2, (max_x - 25) / 2, "*** FORA NO ESPACO ***");
    mvprintw(3, (max_x - 20) / 2, "RESULTADO FINAL");
    attroff(COLOR_PAIR(1) | A_BOLD);
    
    // Calcular estatísticas
    int tempo_jogado = TEMPO_JOGO - estado->tempo_restante;
    float pedidos_por_minuto = tempo_jogado > 0 ? (estado->pedidos_completados * 60.0) / tempo_jogado : 0;
    
    // Determinar motivo do fim
    const char* motivo_fim;
    int cor_motivo;
    if (estado->tempo_restante <= 0) {
        motivo_fim = "*** TEMPO ESGOTADO ***";
        cor_motivo = 3; // Amarelo
    } else if (estado->pedidos && estado->pedidos->count >= MAX_PEDIDOS) {
        motivo_fim = "*** SOBRECARGA DE PEDIDOS ***";
        cor_motivo = 4; // Vermelho
    } else {
        motivo_fim = "*** SAIDA MANUAL ***";
        cor_motivo = 2; // Verde
    }
    
    // Motivo do fim
    attron(COLOR_PAIR(cor_motivo) | A_BOLD);
    mvprintw(5, (max_x - strlen(motivo_fim)) / 2, "%s", motivo_fim);
    attroff(COLOR_PAIR(cor_motivo) | A_BOLD);
    
    // Estatísticas
    attron(COLOR_PAIR(7));
    mvprintw(7, (max_x - 30) / 2, "*** ESTATISTICAS ***");
    
    int minutos = tempo_jogado / 60;
    int segundos = tempo_jogado % 60;
    mvprintw(9, (max_x - 35) / 2, "Tempo jogado: %02d:%02d", minutos, segundos);
    mvprintw(10, (max_x - 35) / 2, "Pedidos completados: %d", estado->pedidos_completados);
    mvprintw(11, (max_x - 35) / 2, "Performance: %.1f pedidos/min", pedidos_por_minuto);
    attroff(COLOR_PAIR(7));
    
    // Classificação
    const char* classificacao;
    const char* nivel;
    int cor_classificacao;
    
    if (estado->pedidos_completados >= 15) {
        classificacao = "EXCELENTE! Voces sao chefs espaciais!";
        nivel = "*** OURO ***";
        cor_classificacao = 3; // Amarelo (ouro)
    } else if (estado->pedidos_completados >= 10) {
        classificacao = "MUITO BOM! Equipe competente!";
        nivel = "*** PRATA ***";
        cor_classificacao = 7; // Branco (prata)
    } else if (estado->pedidos_completados >= 5) {
        classificacao = "BOM! Ainda ha espaco para melhorar!";
        nivel = "*** BRONZE ***";
        cor_classificacao = 3; // Amarelo (bronze)
    } else {
        classificacao = "PRECISA TREINAR! Mais coordenacao!";
        nivel = "*** TREINO ***";
        cor_classificacao = 4; // Vermelho
    }
    
    attron(COLOR_PAIR(cor_classificacao) | A_BOLD);
    mvprintw(13, (max_x - strlen(nivel)) / 2, "%s", nivel);
    mvprintw(14, (max_x - strlen(classificacao)) / 2, "%s", classificacao);
    attroff(COLOR_PAIR(cor_classificacao) | A_BOLD);
    
    // Instruções
    attron(COLOR_PAIR(6));
    mvprintw(max_y - 3, (max_x - 40) / 2, "Pressione qualquer tecla para sair...");
    attroff(COLOR_PAIR(6));
    
    refresh();
    
    // Aguardar tecla
    getch();
    
    // Finalizar ncurses
    finalizar_ncurses();
}