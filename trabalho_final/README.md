# 🚀 Fora no Espaço

## Trabalho Prático de Programação Concorrente

**Instituição:** Instituto Brasileiro de Ensino, Desenvolvimento e Pesquisa (IDP)  
**Disciplina:** Programação Concorrente  
**Professor:** Jeremias Moreira Gomes  
**Período:** 2025/1  
**Alunos:** Lucas Rabello Peixoto Cruz, Matheus Cordeiro Umbelino Lobo

---

## 📋 Descrição

**Fora no Espaço** é um simulador de jogo cooperativo ambientado em uma nave espacial, onde tripulantes trabalham em equipe para preparar pedidos de comida. O foco está no gerenciamento de tarefas concorrentes, recursos compartilhados e sincronização entre threads.

O jogo simula um ambiente onde múltiplos tripulantes competem por recursos limitados (bancadas e cozinhas) para completar pedidos de comida antes que o tempo esgote ou os pedidos se acumulem.

---

## 🎯 Objetivos

- **Primário:** Implementar um sistema com programação concorrente usando threads
- **Educacional:** Compreender sincronização, mutex e gerenciamento de recursos compartilhados
- **Prático:** Desenvolver uma aplicação interativa em tempo real

---

## 🏗️ Arquitetura

### **Threads Principais:**
1. **Mural de Pedidos** - Gera pedidos automaticamente
2. **Exibição de Informações** - Interface visual em tempo real (ncurses)
3. **Chef da Cozinha** - Processa comandos do usuário
4. **Tripulantes** - Executam o trabalho de preparação
5. **Controle de Tempo** - Gerencia duração e fim do jogo

### **Recursos Compartilhados:**
- Lista de pedidos pendentes
- Bancadas de preparação de ingredientes
- Cozinhas de preparo final
- Status dos tripulantes

### **Sincronização:**
- Mutex para cada recurso compartilhado
- Threads-safe para todas as operações
- Prevenção de race conditions

---

## 🛠️ Pré-requisitos

### **Sistema Operacional:**
- Linux (testado em Ubuntu 22.04)
- macOS (testado)

### **Dependências:**
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential libncurses5-dev

# macOS (Homebrew)
brew install ncurses

# Red Hat/CentOS
sudo yum install ncurses-devel gcc

# Arch Linux
sudo pacman -S ncurses gcc
```

### **Compilador:**
- GCC com suporte a C99 e pthreads

---

## 📦 Instalação

### **1. Clonar/Extrair o projeto:**
```bash
# Se usando git
git clone [url-do-repositorio]
cd TrabalhoPC

# Ou extrair ZIP
unzip TrabalhoPC.zip
cd TrabalhoPC
```

### **2. Compilar:**
```bash
make clean
make
```

### **3. Verificar compilação:**
```bash
ls -la fora-no-espaco
# Deve mostrar o executável criado
```

---

## 🎮 Como Jogar

### **Execução:**
```bash
./fora-no-espaco [número_de_tripulantes]
```

**Exemplos:**
```bash
./fora-no-espaco 1    # Jogo com 1 tripulante
./fora-no-espaco 2    # Jogo com 2 tripulantes  
./fora-no-espaco 3    # Jogo com 3 tripulantes
```

### **Interface:**
- **Painel Esquerdo:** Lista de pedidos pendentes
- **Painel Direito:** Status de tripulantes e recursos
- **Barra Superior:** Tempo restante e estatísticas
- **Rodapé:** Comandos disponíveis

### **Comandos:**
| Comando | Ação |
|---------|------|
| `1h` | Tripulante 1 fazer Hamburguer |
| `1s` | Tripulante 1 fazer Suco |
| `1p` | Tripulante 1 fazer Pizza |
| `1c` | Tripulante 1 fazer Cafe |
| `2h` | Tripulante 2 fazer Hamburguer |
| `2s` | Tripulante 2 fazer Suco |
| `2p` | Tripulante 2 fazer Pizza |
| `2c` | Tripulante 2 fazer Cafe |
| `q` | Sair do jogo |

### **Mecânica do Jogo:**

1. **Pedidos são gerados automaticamente:**
   - 1 tripulante: a cada 8 segundos
   - 2 tripulantes: a cada 6 segundos
   - 3 tripulantes: a cada 4 segundos

2. **Fluxo de trabalho:**
   - Digite comando (ex: `1h`)
   - Tripulante 1 pega um pedido de hamburguer
   - **Fase 1:** Preparação de ingredientes (bancada)
   - **Fase 2:** Cozimento (cozinha)
   - Pedido completado!

3. **Sistema de cores:**
   - 🟢 **Verde:** Disponível/Livre
   - 🟡 **Amarelo:** Trabalhando/Tempo médio
   - 🔴 **Vermelho:** Ocupado/Tempo crítico
   - 🟣 **Roxo:** Pedidos novos

---

## ⏱️ Condições de Fim

### **O jogo termina quando:**
1. **Tempo esgota** (2 minutos)
2. **Sobrecarga** (10+ pedidos acumulados)
3. **Saída manual** (tecla 'q')

### **Sistema de Pontuação:**
- **15+ pedidos:** ⭐⭐⭐ EXCELENTE! Vocês são chefs espaciais!
- **10-14 pedidos:** ⭐⭐ MUITO BOM! Equipe competente!
- **5-9 pedidos:** ⭐ BOM! Ainda há espaço para melhorar!
- **<5 pedidos:** ❌ PRECISA TREINAR! Mais coordenação!

---

## 🏗️ Estrutura do Projeto

```
TrabalhoPC/
├── src/
│   ├── main.c          # Função principal e controle
│   ├── game.c          # Lógica central e inicialização
│   ├── mural.c         # Thread de geração de pedidos
│   ├── chef.c          # Thread de processamento de comandos
│   ├── tripulante.c    # Thread dos tripulantes
│   ├── display.c       # Interface visual (ncurses)
│   └── utils.c         # Sistema de log
├── include/
│   ├── game.h          # Estruturas principais
│   ├── mural.h         # Headers do mural
│   ├── chef.h          # Headers do chef
│   ├── tripulante.h    # Headers dos tripulantes
│   ├── display.h       # Headers da interface
│   └── utils.h         # Headers das utilidades
├── Makefile            # Compilação automática
├── README.md           # Esta documentação
└── jogo.log           # Log de debug (criado durante execução)
```

---

## 🔧 Comandos Make

```bash
make            # Compilar o projeto
make clean      # Limpar arquivos objeto
make rebuild    # Limpar e recompilar
make run        # Compilar e executar (com 2 tripulantes)
make debug      # Compilar com flags de debug
```

---

## 🐛 Debugging e Logs

### **Arquivo de Log:**
Durante a execução, um arquivo `jogo.log` é criado com informações de debug:

```bash
# Ver logs em tempo real
tail -f jogo.log

# Ver todo o log
cat jogo.log
```

### **Problemas Comuns:**

**Erro de compilação ncurses:**
```bash
# Ubuntu/Debian
sudo apt install libncurses5-dev

# macOS
brew install ncurses
```

**Programa não responde:**
- Use `Ctrl+C` para finalizar
- Verifique se o terminal suporta ncurses

**Interface visual corrompida:**
- Redimensione a janela do terminal
- Certifique-se de ter pelo menos 80x24 caracteres

---

## 📊 Características Técnicas

### **Programação Concorrente:**
- **5 threads** executando simultaneamente
- **Mutex** para sincronização de recursos
- **Thread-safe** em todas as operações críticas
- **Prevenção de deadlocks** e race conditions

### **Interface:**
- **ncurses** para interface visual
- **Cores dinâmicas** baseadas em estado
- **Atualização em tempo real** (0.5 segundos)
- **Layout responsivo** ao tamanho do terminal

### **Performance:**
- **Geração automática** de pedidos
- **Balanceamento** de carga entre tripulantes
- **Otimização** de uso de recursos
- **Gerenciamento eficiente** de memória

---

## 👥 Desenvolvimento

**Desenvolvido como trabalho acadêmico para a disciplina de Programação Concorrente.**

### **Conceitos Implementados:**
- Threads e paralelismo
- Mutex e sincronização
- Recursos compartilhados
- Interface de usuário em tempo real
- Gerenciamento de estado complexo

### **Tecnologias Utilizadas:**
- **Linguagem:** C (padrão C99)
- **Threads:** POSIX pthreads
- **Interface:** ncurses
- **Build:** Make
- **Sistema:** Linux/macOS

---

## 📝 Licença

Este projeto foi desenvolvido como trabalho acadêmico para fins educacionais.

---

## 🆘 Suporte

Para problemas ou dúvidas:
1. Verifique a seção de **Debugging** acima
2. Consulte os logs em `jogo.log`
3. Certifique-se de que todas as dependências estão instaladas
4. Teste em um terminal com suporte completo a ncurses

---

**Versão:** 1.0  
**Data:** Junho 2025  
**Status:** ✅ Funcional e Testado
