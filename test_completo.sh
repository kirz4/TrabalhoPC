#!/bin/bash

echo "=== TESTE COMPLETO: JOGO FUNCIONANDO ====="

# Cores
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m'

# Compilação
echo -e "${YELLOW}[TESTE 1]${NC} Compilação do jogo completo..."
if make clean && make; then
    echo -e "${GREEN}[OK]${NC} Compilação bem-sucedida!"
else
    echo -e "${RED}[ERRO]${NC} Falha na compilação!"
    exit 1
fi

echo ""
echo -e "${CYAN}=== JOGO FORA NO ESPAÇO - COMPLETO ===${NC}"
echo ""
echo -e "${BLUE}📋 COMO JOGAR:${NC}"
echo ""
echo -e "${MAGENTA}COMANDOS:${NC}"
echo "  ${YELLOW}1h${NC} = Tripulante 1 fazer Hamburguer"
echo "  ${YELLOW}1s${NC} = Tripulante 1 fazer Suco"  
echo "  ${YELLOW}1p${NC} = Tripulante 1 fazer Pizza"
echo "  ${YELLOW}1c${NC} = Tripulante 1 fazer Cafe"
echo ""
echo "  ${YELLOW}2h${NC} = Tripulante 2 fazer Hamburguer"
echo "  ${YELLOW}2s${NC} = Tripulante 2 fazer Suco"
echo "  ${YELLOW}2p${NC} = Tripulante 2 fazer Pizza"
echo "  ${YELLOW}2c${NC} = Tripulante 2 fazer Cafe"
echo ""
echo "  ${YELLOW}q${NC}  = Sair do jogo"
echo ""
echo -e "${BLUE}🎮 MECÂNICA DO JOGO:${NC}"
echo "1. Pedidos são gerados automaticamente"
echo "2. Digite comandos para atribuir pedidos aos tripulantes"
echo "3. Tripulantes passam por 2 fases:"
echo "   • ${CYAN}Preparação de ingredientes${NC} (na bancada)"
echo "   • ${CYAN}Cozimento${NC} (na cozinha)"
echo "4. Observe as cores mudarem conforme o status"
echo "5. Pedidos antigos ficam vermelhos (urgentes!)"
echo ""
echo -e "${BLUE}🌈 CÓDIGO DE CORES:${NC}"
echo "🟢 Verde  = Livre/Disponível"
echo "🟡 Amarelo = Trabalhando/Médio"  
echo "🔴 Vermelho = Ocupado/Urgente"
echo "🟣 Roxo   = Pedidos novos"
echo ""
echo -e "${YELLOW}EXEMPLO: Digite '1h' para o Tripulante 1 fazer um Hamburguer${NC}"
echo ""
echo -e "${GREEN}Iniciando jogo com 2 tripulantes...${NC}"
echo -e "${BLUE}Pressione 'q' para sair quando quiser${NC}"
echo ""

# Executar o jogo
./fora-no-espaco 2

echo ""
echo -e "${GREEN}=== JOGO FINALIZADO! ===${NC}"
echo ""
echo -e "${BLUE}O que você deve ter experimentado:${NC}"
echo "✅ Interface visual em tempo real"
echo "✅ Comandos funcionando (ex: 1h, 2s, etc.)"
echo "✅ Tripulantes trabalhando automaticamente"
echo "✅ Recursos sendo ocupados e liberados"
echo "✅ Pedidos sendo completados"
echo "✅ Contador de pedidos completados aumentando"
echo "✅ Sistema de cores dinâmico"
echo ""
echo -e "${YELLOW}Próximas fases: Polimento e Documentação${NC}"