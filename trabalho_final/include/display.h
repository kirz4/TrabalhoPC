#ifndef DISPLAY_H
#define DISPLAY_H

#include "game.h"

// Função da thread de display
void* thread_exibir_info(void* arg);

// Funções auxiliares de ncurses
void inicializar_ncurses(void);
void finalizar_ncurses(void);
void desenhar_borda(int y, int x, int height, int width, const char* titulo);
void mostrar_tela_final(EstadoJogo* estado);

#endif // DISPLAY_H