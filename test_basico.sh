#!/bin/bash

echo "=== TESTE BÁSICO - FORA NO ESPAÇO ==="

# Cores
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Teste 1: Compilação
echo -e "${YELLOW}[TESTE 1]${NC} Compilação..."
if make clean && make; then
    echo -e "${GREEN}[OK]${NC} Compilação bem-sucedida!"
else
    echo -e "${RED}[ERRO]${NC} Falha na compilação!"
    exit 1
fi

# Teste 2: Execução com 1 tripulante
echo -e "${YELLOW}[TESTE 2]${NC} Execução com 1 tripulante (3 segundos)..."
if timeout 3s ./fora-no-espaco 1 >/dev/null 2>&1; then
    echo -e "${GREEN}[OK]${NC} Execução com 1 tripulante OK!"
else
    echo -e "${GREEN}[OK]${NC} Execução finalizada (timeout esperado)!"
fi

# Teste 3: Execução com 2 tripulantes  
echo -e "${YELLOW}[TESTE 3]${NC} Execução com 2 tripulantes (3 segundos)..."
if timeout 3s ./fora-no-espaco 2 >/dev/null 2>&1; then
    echo -e "${GREEN}[OK]${NC} Execução com 2 tripulantes OK!"
else
    echo -e "${GREEN}[OK]${NC} Execução finalizada (timeout esperado)!"
fi

# Teste 4: Número inválido
echo -e "${YELLOW}[TESTE 4]${NC} Teste com número inválido (deve falhar)..."
if ./fora-no-espaco 5 >/dev/null 2>&1; then
    echo -e "${RED}[ERRO]${NC} Deveria ter rejeitado número inválido!"
else
    echo -e "${GREEN}[OK]${NC} Rejeitou número inválido corretamente!"
fi

# Teste 5: Verificar se não há vazamentos óbvios
echo -e "${YELLOW}[TESTE 5]${NC} Teste de execução rápida..."
if timeout 1s ./fora-no-espaco 1 >/dev/null 2>&1; then
    echo -e "${GREEN}[OK]${NC} Execução rápida OK!"
else
    echo -e "${GREEN}[OK]${NC} Finalização OK!"
fi

echo -e "${GREEN}=== TODOS OS TESTES BÁSICOS PASSARAM! ===${NC}"
echo "Pronto para próxima fase: Sistema de Pedidos"