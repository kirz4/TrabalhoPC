#!/bin/bash

echo "=== TESTE FASE 3: INTERFACE VISUAL ====="

# Cores
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

# Compilação
echo -e "${YELLOW}[TESTE 1]${NC} Compilação da interface visual..."
if make clean && make; then
    echo -e "${GREEN}[OK]${NC} Compilação bem-sucedida!"
else
    echo -e "${RED}[ERRO]${NC} Falha na compilação!"
    exit 1
fi

echo ""
echo -e "${CYAN}=== INTERFACE VISUAL COM NCURSES ===${NC}"
echo ""
echo -e "${BLUE}A interface visual apresentará:${NC}"
echo "🚀 Cabeçalho: 'FORA NO ESPAÇO'"
echo "⏱️  Informações: Tempo, pedidos completados, número de tripulantes"
echo "📋 Painel esquerdo: Lista de pedidos pendentes (com cores por tempo)"
echo "👥 Painel direito: Status dos tripulantes e recursos"
echo "🎮 Rodapé: Comandos disponíveis"
echo ""
echo -e "${YELLOW}Cores da interface:${NC}"
echo "🟢 Verde: Recursos livres / Tripulantes disponíveis"
echo "🟡 Amarelo: Tripulantes trabalhando / Pedidos antigos"
echo "🔴 Vermelho: Recursos ocupados / Pedidos muito antigos"
echo "🟣 Roxo: Pedidos novos"
echo "🔵 Azul: Destaques e comandos"
echo ""
echo -e "${CYAN}TESTE COM 2 TRIPULANTES:${NC}"
echo "Observe os pedidos sendo gerados e as cores mudando conforme o tempo..."
echo ""
echo -e "${BLUE}Pressione 'q' ou Ctrl+C para sair${NC}"
echo ""

# Executar com interface visual
./fora-no-espaco 2

echo ""
echo -e "${GREEN}=== TESTE DA INTERFACE VISUAL CONCLUÍDO! ===${NC}"
echo ""
echo -e "${BLUE}O que você deve ter visto:${NC}"
echo "✅ Interface dividida em dois painéis"
echo "✅ Pedidos listados com cores baseadas no tempo de espera"
echo "✅ Status dos tripulantes atualizado em tempo real"
echo "✅ Status das bancadas e cozinhas"
echo "✅ Atualização suave da tela (0.5s)"
echo "✅ Comandos mostrados no rodapé"
echo ""
echo -e "${YELLOW}Próxima fase: Lógica dos Tripulantes${NC}"