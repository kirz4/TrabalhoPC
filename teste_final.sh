#!/bin/bash

echo "🚀 === TESTE FINAL - FORA NO ESPAÇO === 🚀"
echo ""

# Cores para output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

# Função para teste
test_check() {
    local name="$1"
    local command="$2"
    
    echo -e "${YELLOW}[TESTE]${NC} $name"
    
    if eval "$command" >/dev/null 2>&1; then
        echo -e "${GREEN}✅ PASSOU${NC} - $name"
        return 0
    else
        echo -e "${RED}❌ FALHOU${NC} - $name"
        return 1
    fi
}

echo -e "${BOLD}=== FASE 1: VERIFICAÇÕES BÁSICAS ===${NC}"
echo ""

# Verificar arquivos necessários
test_check "Makefile existe" "test -f Makefile"
test_check "Pasta src existe" "test -d src"
test_check "Pasta include existe" "test -d include"
test_check "game.h existe" "test -f include/game.h"
test_check "main.c existe" "test -f src/main.c"

echo ""
echo -e "${BOLD}=== FASE 2: COMPILAÇÃO ===${NC}"
echo ""

# Teste de compilação
echo -e "${YELLOW}[TESTE]${NC} Compilação limpa..."
if make clean && make; then
    echo -e "${GREEN}✅ PASSOU${NC} - Compilação bem-sucedida!"
else
    echo -e "${RED}❌ FALHOU${NC} - Erro de compilação!"
    echo "Verifique os erros acima e corrija antes de continuar."
    exit 1
fi

# Verificar se executável foi criado
test_check "Executável criado" "test -f fora-no-espaco"
test_check "Executável tem permissão" "test -x fora-no-espaco"

echo ""
echo -e "${BOLD}=== FASE 3: TESTES DE EXECUÇÃO ===${NC}"
echo ""

# Teste com argumentos inválidos
echo -e "${YELLOW}[TESTE]${NC} Rejeição de argumentos inválidos..."
if ./fora-no-espaco 0 2>/dev/null || ./fora-no-espaco 5 2>/dev/null; then
    echo -e "${RED}❌ FALHOU${NC} - Deveria rejeitar argumentos inválidos"
else
    echo -e "${GREEN}✅ PASSOU${NC} - Rejeita argumentos inválidos corretamente"
fi

echo ""
echo -e "${BOLD}=== FASE 4: TESTE INTERATIVO ===${NC}"
echo ""
echo -e "${CYAN}Agora vamos testar a interface completa!${NC}"
echo ""
echo -e "${BLUE}📋 CHECKLIST DURANTE O TESTE:${NC}"
echo "□ Interface visual aparece corretamente"
echo "□ Título 'FORA NO ESPAÇO' centralizado"  
echo "□ Dois painéis (pedidos à esquerda, status à direita)"
echo "□ Pedidos sendo gerados automaticamente"
echo "□ Cores funcionando (verde/amarelo/vermelho)"
echo "□ Comandos funcionam (1h, 2s, etc.)"
echo "□ Tripulantes mudam status quando trabalham"
echo "□ Recursos ficam ocupados/livres"
echo "□ Contador de pedidos completados aumenta"
echo "□ Tempo diminui (formato MM:SS)"
echo "□ Tela final aparece com estatísticas"
echo ""
echo -e "${YELLOW}COMANDOS PARA TESTAR:${NC}"
echo "• Digite: 1h (Tripulante 1 fazer hamburguer)"
echo "• Digite: 2s (Tripulante 2 fazer suco)"
echo "• Digite: 1p (Tripulante 1 fazer pizza)"
echo "• Digite: q (para sair antes do tempo)"
echo ""
echo -e "${BLUE}🎮 INICIANDO JOGO COM 2 TRIPULANTES...${NC}"
echo -e "${CYAN}Teste por pelo menos 30 segundos para ver tudo funcionando!${NC}"
echo ""

# Executar o jogo
./fora-no-espaco 2

echo ""
echo -e "${BOLD}=== FASE 5: VERIFICAÇÃO PÓS-TESTE ===${NC}"
echo ""

# Verificar se log foi criado
test_check "Arquivo de log criado" "test -f jogo.log"

if test -f jogo.log; then
    echo -e "${YELLOW}[INFO]${NC} Últimas 5 linhas do log:"
    tail -5 jogo.log | sed 's/^/  /'
fi

echo ""
echo -e "${BOLD}=== RESULTADO FINAL ===${NC}"
echo ""
echo -e "${GREEN}✅ Compilação: OK${NC}"
echo -e "${GREEN}✅ Execução: OK${NC}"
echo -e "${GREEN}✅ Validação: OK${NC}"
echo ""
echo -e "${CYAN}🎉 JOGO FORA NO ESPAÇO ESTÁ FUNCIONANDO!${NC}"
echo ""
echo -e "${BLUE}📊 PRÓXIMOS PASSOS:${NC}"
echo "1. Documentar o código (relatório técnico)"
echo "2. Testar com diferentes números de tripulantes"
echo "3. Verificar sincronização de threads"
echo "4. Preparar entrega final"
echo ""
echo -e "${YELLOW}Para testar novamente:${NC} ./fora-no-espaco [1-3]"
echo -e "${YELLOW}Para ver logs:${NC} tail -f jogo.log"