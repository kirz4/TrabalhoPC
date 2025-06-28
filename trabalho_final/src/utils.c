// utils.c - Sistema de log para não interferir com ncurses

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"

static FILE* log_file = NULL;
static bool log_habilitado = false;

// Inicializar sistema de log
void inicializar_log(const char* arquivo) {
    log_file = fopen(arquivo, "w");
    if (log_file != NULL) {
        log_habilitado = true;
        log_debug("Sistema de log iniciado");
    }
}

// Finalizar sistema de log
void finalizar_log() {
    if (log_file != NULL) {
        log_debug("Sistema de log finalizado");
        fclose(log_file);
        log_file = NULL;
    }
    log_habilitado = false;
}

// Função de log que não interfere com ncurses
void log_debug(const char* formato, ...) {
    if (!log_habilitado || log_file == NULL) {
        return;
    }
    
    // Timestamp
    time_t agora = time(NULL);
    struct tm* tm_info = localtime(&agora);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm_info);
    
    // Escrever timestamp
    fprintf(log_file, "[%s] ", timestamp);
    
    // Escrever mensagem formatada
    va_list args;
    va_start(args, formato);
    vfprintf(log_file, formato, args);
    va_end(args);
    
    fprintf(log_file, "\n");
    fflush(log_file); // Forçar escrita imediata
}