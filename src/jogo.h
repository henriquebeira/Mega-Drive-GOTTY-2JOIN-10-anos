#ifndef JOGO_H
#define JOGO_H

#include <genesis.h>
#include "resources.h"

#define MAX_TIROS 4
#define MAX_TIROS_INIMIGO 4
#define SFX_TIRO_ID 64
#define BGM_MENU_ID 65
#define BGM_JOGO_ID 66

#define TIPO_N64 0
#define TIPO_ARANHA 1

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
    bool ativo;
    u16 timerRespawn;
    u16 timerTiro;
} Inimigo;

void atualizarPlacar(u16 pontos);
void atualizarContadorEdicao(u16 edicao);
void spawnInimigo(Inimigo* inimigo, u16 edicao);
void atualizarInimigoETiros(Inimigo* inimigo, Tiro tirosInimigo[], s16 playerX, s16 playerY, u16* timerPiscaDano, u16 edicao);

#endif
