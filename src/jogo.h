#ifndef JOGO_H
#define JOGO_H

#include <genesis.h>
#include "resources.h"

#define MAX_TIROS 4
#define MAX_TIROS_INIMIGO 4
#define SFX_TIRO_ID 64
#define BGM_MENU_ID 65
#define BGM_JOGO_ID 66

typedef struct {
    Sprite* sprite;
    s16 x;
    s16 y;
    s16 vy;
    bool ativo;
} Tiro;

typedef struct {
    Sprite* sprite;
    s16 x;
    s16 y;
    bool ativo;
    u16 timerRespawn;
    u16 timerTiro;
} Inimigo;

void atualizarPlacar(u16 pontos);
void spawnInimigo(Inimigo* inimigo);
void atualizarInimigoETiros(Inimigo* inimigo, Tiro tirosInimigo[], s16 playerX, s16 playerY, u16* timerPiscaDano);

#endif
