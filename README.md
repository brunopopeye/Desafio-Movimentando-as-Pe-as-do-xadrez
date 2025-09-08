# Desafio – Movimentando as Peças do Xadrez (C)

Projeto em C (console) para validar e executar **movimentos legais** das peças de xadrez.

## O que está implementado
- Movimentos legais de **peão, cavalo, bispo, torre, dama e rei**;
- **Bloqueio de caminho** para peças deslizantes (bispo/torre/dama);
- **Capturas**, **alternância de turnos** e **promoção simples** (peão vira dama ao chegar na última fileira);
- **Entrada** no formato `e2 e4` (algebraico simples);
- Tabuleiro inicial padrão.

> **Não** implementa: xeque/xeque-mate, **en passant** e **roque** (podem ser adicionados depois).

## Como compilar

### Linux/macOS
```bash
make
./bin/chess
Como usar

O programa mostra o tabuleiro: maiúsculas = brancas, minúsculas = pretas, . = vazio.

Digite lances assim:

e2 e4
g8 f6
a7 a5
Para sair, digite q.

Critérios de movimento (resumo)

Peão: anda 1 casa à frente; 2 casas se na fileira inicial (2ª dos pretos / 7ª dos brancos) e caminho livre; captura diagonal 1 casa; promoção automática ao fim.

Cavalo: em “L” (2+1); pode pular peças.

Bispo: diagonais, caminho livre.

Torre: horizontal/vertical, caminho livre.

Dama: torre + bispo, caminho livre.

Rei: 1 casa em qualquer direção (sem roque por enquanto).
