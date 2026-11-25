#include "jogo.h"

void spawnInimigo(Inimigo* inimigo) {
    inimigo->x = 16 + (random() % 265); // Entre 16 e 280
    inimigo->y = 28 + (random() % 77);  // Entre 28 e 104 (metade superior)
    inimigo->ativo = TRUE;
    inimigo->timerTiro = 60;            // Atira após 1 segundo (60 frames)
    SPR_setPosition(inimigo->sprite, inimigo->x, inimigo->y);
    SPR_setVisibility(inimigo->sprite, VISIBLE);
}

void atualizarInimigoETiros(Inimigo* inimigo, Tiro tirosInimigo[], s16 playerX, s16 playerY, u16* timerPiscaDano) {
    // Disparo a cada 1 segundo ou contagem para renascer
    if (inimigo->ativo) {
        if (inimigo->timerTiro > 0) {
            inimigo->timerTiro--;
        } else {
            inimigo->timerTiro = 60;

            for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
                if (!tirosInimigo[i].ativo) {
                    tirosInimigo[i].ativo = TRUE;
                    tirosInimigo[i].x = inimigo->x + 4;
                    tirosInimigo[i].y = inimigo->y + 20;
                    tirosInimigo[i].vy = 3;

                    SPR_setPosition(tirosInimigo[i].sprite, tirosInimigo[i].x, tirosInimigo[i].y);
                    SPR_setVisibility(tirosInimigo[i].sprite, VISIBLE);
                    break;
                }
            }
        }
    } else {
        if (inimigo->timerRespawn > 0) {
            inimigo->timerRespawn--;
        } else {
            spawnInimigo(inimigo);
        }
    }

    // Movimento dos tiros inimigos e colisão com o jogador
    for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
        if (tirosInimigo[i].ativo) {
            tirosInimigo[i].y += tirosInimigo[i].vy;

            if (tirosInimigo[i].y > 224) {
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
