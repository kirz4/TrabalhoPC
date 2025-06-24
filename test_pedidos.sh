#!/bin/bash

echo "=== TESTE FASE 2: SISTEMA DE PEDIDOS ==="

# Cores
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Compilação
echo -e "${YELLOW}[TESTE 1]${NC} Compilação do sistema de pedidos..."
if make clean && make; then
    echo -e "${GREEN}[OK]${NC} Compilação bem-sucedida!"
else
    echo -e "${RED}[ERRO]${NC} Falha na compilação!"
    exit 1
fi

# Teste de execução com visualização
echo -e "${YELLOW}[TESTE 2]${NC} Execução com 1 tripulante (15 segundos)..."
echo -e "${BLUE}Você deve ver:${NC}"
echo "  - Mensagens de inicialização"
echo "  - Pedidos sendo criados automaticamente"
echo "  - Status do jogo sendo atualizado"
echo "  - Lista de pedidos pendentes"
echo ""
echo -e "${BLUE}Pressione Ctrl+C para parar quando quiser...${NC}"
echo ""

# Executar por 15 segundos ou até o usuário parar
timeout 15s ./fora-no-espaco 1 || true

echo ""
echo -e "${YELLOW}[TESTE 3]${NC} Execução com 2 tripulantes (10 segundos)..."
echo -e "${BLUE}Agora com 2 tripulantes (pedidos mais frequentes)...${NC}"
echo ""

timeout 10s ./fora-no-espaco 2 || true

echo ""
echo -e "${YELLOW}[TESTE 4]${NC} Teste rápido com 3 tripulantes (5 segundos)..."
echo -e "${BLUE}Com 3 tripulantes (pedidos muito frequentes)...${NC}"
echo ""

timeout 5s ./fora-no-espaco 3 || true

echo ""
echo -e "${GREEN}=== TESTES DA FASE 2 CONCLUÍDOS! ===${NC}"
echo ""
echo -e "${BLUE}O que você deve ter observado:${NC}"
echo "✅ Pedidos sendo gerados automaticamente"
echo "✅ Diferentes frequências baseadas no número de tripulantes"
echo "✅ Status do jogo sendo atualizado continuamente"
echo "✅ Lista de pedidos pendentes sendo mostrada"
echo "✅ Contadores funcionando (tempo, pedidos, etc.)"
echo ""
echo -e "${YELLOW}Próxima fase: Interface Visual com ncurses${NC}"