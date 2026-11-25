#ifndef JOGO_H
#define JOGO_H

#include <genesis.h>
#include "resources.h"

#define MAX_TIROS 4
#define MAX_TIROS_INIMIGO 10
#define SFX_TIRO_ID 64
#define BGM_MENU_ID 65
#define BGM_JOGO_ID 66

#define TIPO_N64 0
#define TIPO_ARANHA 1
#define TIPO_DISJUNTOR 2

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

// Funções de inimigos (src/inimigos.c)
void spawnN64(Inimigo* n64);
void spawnAranha(Inimigo* aranha);
void spawnDisjuntor(Inimigo* disjuntor, s16 playerX);
void atualizarInimigosETiros(Inimigo* n64, Inimigo* aranha, Inimigo* disjuntor, Tiro tirosInimigo[], s16 playerX, s16 playerY, u16* timerPiscaDano, u16 edicao);

#endif
