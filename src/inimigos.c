#include "jogo.h"

void spawnInimigo(Inimigo* inimigo, u16 edicao) {
    // Após 30 2JOIN, sorteia entre N64 (0) e Aranha (1)
    if (edicao > 30 && (random() % 2 == 1)) {
        inimigo->tipo = TIPO_ARANHA;
        inimigo->largura = 32;
        inimigo->altura = 32;
        PAL_setPalette(PAL3, spr_aranha.palette->data, DMA);
        SPR_setDefinition(inimigo->sprite, &spr_aranha);
    } else {
        inimigo->tipo = TIPO_N64;
        inimigo->largura = 24;
        inimigo->altura = 24;
        PAL_setPalette(PAL3, spr_n64.palette->data, DMA);
        SPR_setDefinition(inimigo->sprite, &spr_n64);
    }

    // Garante que caiba na tela independentemente da largura (24 ou 32)
    s16 maxSpawnX = 320 - inimigo->largura - 32;
    inimigo->x = 16 + (random() % (maxSpawnX + 1));
    inimigo->y = 28 + (random() % 77); // Metade superior da tela
    inimigo->ativo = TRUE;
    inimigo->timerTiro = 60;           // Atira após 1 segundo (60 frames)

    SPR_setPosition(inimigo->sprite, inimigo->x, inimigo->y);
    SPR_setVisibility(inimigo->sprite, VISIBLE);
}

void atualizarInimigoETiros(Inimigo* inimigo, Tiro tirosInimigo[], s16 playerX, s16 playerY, u16* timerPiscaDano, u16 edicao) {
    if (inimigo->ativo) {
        if (inimigo->timerTiro > 0) {
            inimigo->timerTiro--;
        } else {
            inimigo->timerTiro = 60;

            for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
                if (!tirosInimigo[i].ativo) {
                    tirosInimigo[i].ativo = TRUE;

                    if (inimigo->tipo == TIPO_N64) {
                        // N64: Tiro vertical saindo do pino inferior
                        SPR_setDefinition(tirosInimigo[i].sprite, &spr_tiro_inimigo_vertical);
                        tirosInimigo[i].x = inimigo->x + 4;
                        tirosInimigo[i].y = inimigo->y + 20;
                        tirosInimigo[i].vx = 0;
                        tirosInimigo[i].vy = 3;
                    } else {
                        // Aranha: Esfera de fogo mirando na direção do jogador
                        SPR_setDefinition(tirosInimigo[i].sprite, &spr_tiro_inimigo_bola);
                        tirosInimigo[i].x = inimigo->x + 8;  // Centro horizontal da Aranha (32 - 16) / 2
                        tirosInimigo[i].y = inimigo->y + 24; // Parte inferior da Aranha

                        s16 centroAlvoX = playerX + 24;
                        s16 centroAlvoY = playerY + 24;
                        s16 centroTiroX = tirosInimigo[i].x + 8;
                        s16 centroTiroY = tirosInimigo[i].y + 8;

                        s16 dx = centroAlvoX - centroTiroX;
                        s16 dy = centroAlvoY - centroTiroY;
                        if (dy < 16) dy = 16; // Garante que o tiro sempre desça

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
    } else {
        if (inimigo->timerRespawn > 0) {
            inimigo->timerRespawn--;
        } else {
            spawnInimigo(inimigo, edicao);
        }
    }

    // Movimento dos tiros inimigos (horizontal + vertical) e colisão com o jogador
    for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
        if (tirosInimigo[i].ativo) {
            tirosInimigo[i].x += tirosInimigo[i].vx;
            tirosInimigo[i].y += tirosInimigo[i].vy;

            if (tirosInimigo[i].y > 224 || tirosInimigo[i].x < -16 || tirosInimigo[i].x > 320) {
                SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
                tirosInimigo[i].ativo = FALSE;
            } else {
                SPR_setPosition(tirosInimigo[i].sprite, tirosInimigo[i].x, tirosInimigo[i].y);

                // Hitbox central (16x16) do jogador (48x48)
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
