#ifndef JOGO_H
#define JOGO_H

#include <genesis.h>
#include "resources.h"

#define MAX_TIROS 4
#define MAX_TIROS_INIMIGO 12
#define SFX_TIRO_ID 64
#define BGM_MENU_ID 65
#define BGM_JOGO_ID 66
#define SFX_TOASTY_ID 67
#define SFX_DANO_ID 68
#define SFX_EXPLOSAO_ID 69

#define TIPO_N64 0
#define TIPO_ARANHA 1
#define TIPO_DISJUNTOR 2
#define TIPO_PNEU 3

typedef struct {
    Sprite* sprite;
    s16 x;
    s16 y;
    bool andando;
    bool olhandoDireita;
    u8 cooldownTiro;
    u16 timerPiscaDano;
// Adicione dentro de typedef struct { ... } Player; no src/jogo.h:
    u8 multiplicador;     // 1 a 8
    u8 killsNoNivel;      // Quantos inimigos derrotou no nível atual do combo
    u8 metaProximoNivel;  // 4 para 2x, 6 para 3x, 8 para 4x...
    Sprite* spriteMult;   // Ícone 2X..8X acima da barra
    Sprite* spriteToasty;
    u8 timerToasty;       // Controla a subida e descida no canto inferior direito
} Player;

typedef struct {
    Sprite* sprite;
    s16 x;
    s16 y;
    s16 vx;
    s16 vy;
    bool ativo;
} Tiro;

typedef struct {
    Sprite* sprite;
    s16 x;
    s16 y;
    s8 vx;
    s8 vy;
    u8 tipo;
    u8 largura;
    u8 altura;
    u8 hp;
    bool ativo;
    u16 timerRespawn;
    u16 timerTiro;
    u16 timerPisca;
} Inimigo;

// Funções de telas e HUD (src/telas.c)
void atualizarPlacar(u16 pontos);
void atualizarContadorEdicao(u16 edicao);
void executarTelaInicial(void);
void executarTelaFinal(u16 pontos);

// Funções do jogador e tiros (src/player.c)
void inicializarPlayer(Player* p, Tiro tiros[]);
void atualizarPlayerETiros(Player* p, Tiro tiros[], Inimigo* n64, Inimigo* aranha, Inimigo* disjuntor, Inimigo* pneu, u16* pontos);

// Funções de inimigos (src/inimigos.c)
void spawnN64(Inimigo* n64);
void spawnAranha(Inimigo* aranha);
void spawnDisjuntor(Inimigo* disjuntor, s16 playerX);
void spawnPneu(Inimigo* pneu);

void atualizarInimigosETiros(Inimigo* n64, Inimigo* aranha, Inimigo* disjuntor, Inimigo* pneu, Tiro tirosInimigo[], Player* player, u16 edicao);
void inicializarTilesBarraCombo(void);
void atualizarBarraCombo(Player* p);
void registrarAbateCombo(Player* p);
void resetarCombo(Player* p);

void atualizarAnimacaoToasty(Player* p);
HINTERRUPT_CALLBACK onHBlankToasty(void);

void tocarSomDanoPSG(void);
void atualizarSomDanoPSG(void);

#endif
