#include "jogo.h"

void spawnN64(Inimigo* n64) {
    n64->tipo = TIPO_N64;
    n64->largura = 24;
    n64->altura = 24;
    n64->x = 16 + (random() % 265); // Entre 16 e 280
    n64->y = 28 + (random() % 77);  // Metade superior da tela
    n64->ativo = TRUE;
    n64->timerTiro = 60;            // Atira a cada 1 segundo (60 frames)

    SPR_setPosition(n64->sprite, n64->x, n64->y);
    SPR_setVisibility(n64->sprite, VISIBLE);
}

void spawnAranha(Inimigo* aranha) {
    aranha->tipo = TIPO_ARANHA;
    aranha->largura = 32;
    aranha->altura = 32;
    aranha->x = 16 + (random() % 257); // Entre 16 e 272
    aranha->y = 28 + (random() % 69);  // Metade superior da tela
    aranha->ativo = TRUE;
    aranha->timerTiro = 75;            // Cadência intercalada com o N64

    SPR_setPosition(aranha->sprite, aranha->x, aranha->y);
    SPR_setVisibility(aranha->sprite, VISIBLE);
}

static void dispararTiroInimigo(Inimigo* inimigo, Tiro tirosInimigo[], s16 playerX, s16 playerY) {
    for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
        if (!tirosInimigo[i].ativo) {
            tirosInimigo[i].ativo = TRUE;

            if (inimigo->tipo == TIPO_N64) {
                              SPR_setAnim(tirosInimigo[i].sprite, 1);
                tirosInimigo[i].x = inimigo->x + 4;
                tirosInimigo[i].y = inimigo->y + 20;
                tirosInimigo[i].vx = 0;
                tirosInimigo[i].vy = 3;
            } else {
                              SPR_setAnim(tirosInimigo[i].sprite, 2);
                tirosInimigo[i].x = inimigo->x + 8;
                tirosInimigo[i].y = inimigo->y + 24;

                s16 centroAlvoX = playerX + 24;
                s16 centroAlvoY = playerY + 24;
                s16 centroTiroX = tirosInimigo[i].x + 8;
                s16 centroTiroY = tirosInimigo[i].y + 8;

                s16 dx = centroAlvoX - centroTiroX;
                s16 dy = centroAlvoY - centroTiroY;
                if (dy < 16) dy = 16;

                s16 dist = abs(dx) + abs(dy);
                s16 velocidade = 3;

                tirosInimigo[i].vx = (dx * velocidade) / dist;
                tirosInimigo[i].vy = (dy * velocidade) / dist;
                if (tirosInimigo[i].vy < 1) tirosInimigo[i].vy = 1;
            }

            SPR_setPosition(tirosInimigo[i].sprite, tirosInimigo[i].x, tirosInimigo[i].y);
            SPR_setVisibility(tirosInimigo[i].sprite, VISIBLE);
            break;
        }
    }
}

void atualizarInimigosETiros(Inimigo* n64, Inimigo* aranha, Tiro tirosInimigo[], s16 playerX, s16 playerY, u16* timerPiscaDano, u16 edicao) {
    // 1. Lógica do N64
    if (n64->ativo) {
        if (n64->timerTiro > 0) {
            n64->timerTiro--;
        } else {
            n64->timerTiro = 60;
            dispararTiroInimigo(n64, tirosInimigo, playerX, playerY);
        }
    } else {
        if (n64->timerRespawn > 0) {
            n64->timerRespawn--;
        } else {
            spawnN64(n64);
        }
    }

    // 2. Lógica da Aranha (após 30 2JOIN)
    if (edicao > 30) {
        if (aranha->ativo) {
            if (aranha->timerTiro > 0) {
                aranha->timerTiro--;
            } else {
                aranha->timerTiro = 75;
                dispararTiroInimigo(aranha, tirosInimigo, playerX, playerY);
            }
        } else {
            if (aranha->timerRespawn > 0) {
                aranha->timerRespawn--;
            } else {
                spawnAranha(aranha);
            }
        }
    }

    // 3. Movimento dos tiros inimigos e colisão com o jogador
    for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
        if (tirosInimigo[i].ativo) {
            tirosInimigo[i].x += tirosInimigo[i].vx;
            tirosInimigo[i].y += tirosInimigo[i].vy;

            if (tirosInimigo[i].y > 224 || tirosInimigo[i].x < -16 || tirosInimigo[i].x > 320) {
                SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
                tirosInimigo[i].ativo = FALSE;
            } else {
                SPR_setPosition(tirosInimigo[i].sprite, tirosInimigo[i].x, tirosInimigo[i].y);

                if (tirosInimigo[i].x + 12 > playerX + 16 && tirosInimigo[i].x + 4 < playerX + 32 &&
                    tirosInimigo[i].y + 12 > playerY + 16 && tirosInimigo[i].y + 4 < playerY + 32)
                {
                    tirosInimigo[i].ativo = FALSE;
                    SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
                    *timerPiscaDano = 30;
                }
            }
        }
    }
}
