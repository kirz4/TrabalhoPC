# Makefile para o projeto Fora no Espaço
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread
LIBS = -lncurses -lpthread
SRCDIR = src
INCDIR = include
OBJDIR = obj

# Nome do executável
TARGET = fora-no-espaco

# Arquivos fonte
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Inclui diretórios de header
INCLUDES = -I$(INCDIR)

# Regra principal
all: $(TARGET)

# Criação do executável
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

# Compilação dos objetos
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Criação do diretório obj
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Limpeza
clean:
	rm -rf $(OBJDIR) $(TARGET)

# Reinstalação
rebuild: clean all

# Execução
run: $(TARGET)
	./$(TARGET)

# Debug
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

.PHONY: all clean rebuild run debug