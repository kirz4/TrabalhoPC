#ifndef UTILS_H
#define UTILS_H

// Sistema de log (não interfere com ncurses)
void inicializar_log(const char* arquivo);
void finalizar_log(void);
void log_debug(const char* formato, ...);

#endif // UTILS_H