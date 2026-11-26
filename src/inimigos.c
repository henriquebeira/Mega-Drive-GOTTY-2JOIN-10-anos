#include "jogo.h"

void spawnN64(Inimigo* n64) {
    n64->tipo = TIPO_N64;
    n64->largura = 24;
    n64->altura = 24;
    n64->hp = 1;
n64->x = 16 + (random() % 265);
n64->y = 36 + (random() % 60);
n64->baseX = n64->x;
n64->baseY = n64->y;
n64->vx = 0;
n64->vy = 1;
    n64->ativo = TRUE;
    n64->timerTiro = 60;
    n64->timerPisca = 0;

    SPR_setPosition(n64->sprite, n64->x, n64->y);
    SPR_setVisibility(n64->sprite, VISIBLE);
}

void spawnAranha(Inimigo* aranha) {
    aranha->tipo = TIPO_ARANHA;
    aranha->largura = 32;
    aranha->altura = 32;
    aranha->hp = 2;
aranha->x = 32 + (random() % 225);
aranha->y = 28 + (random() % 69);
aranha->baseX = aranha->x;
aranha->baseY = aranha->y;
aranha->vx = 1;
aranha->vy = 0;
    aranha->ativo = TRUE;
    aranha->timerTiro = 75;
    aranha->timerPisca = 0;

    SPR_setPosition(aranha->sprite, aranha->x, aranha->y);
    SPR_setVisibility(aranha->sprite, VISIBLE);
}

void spawnDisjuntor(Inimigo* disjuntor, s16 playerX) {
    disjuntor->tipo = TIPO_DISJUNTOR;
    disjuntor->largura = 48;
    disjuntor->altura = 32;
    disjuntor->hp = 3;
    disjuntor->x = playerX;
    if (disjuntor->x < 0) disjuntor->x = 0;
    if (disjuntor->x > 272) disjuntor->x = 272;
    disjuntor->y = 137;
    disjuntor->vx = 0;
    disjuntor->vy = 0;
    disjuntor->ativo = TRUE;
    disjuntor->timerTiro = 0;
    disjuntor->timerPisca = 0;

    SPR_setPosition(disjuntor->sprite, disjuntor->x, disjuntor->y);
    SPR_setVisibility(disjuntor->sprite, VISIBLE);
}

void spawnPneu(Inimigo* pneu) {
    pneu->tipo = TIPO_PNEU;
    pneu->largura = 32;
    pneu->altura = 24;
    pneu->hp = 1;
    pneu->x = 320;
    pneu->y = 198;
    pneu->vx = -1; // Travessia horizontal mais lenta (1px por frame)
    pneu->vy = -4; // Impulso inicial mais suave
    pneu->ativo = TRUE;
    pneu->timerTiro = 0;
    pneu->timerPisca = 0;

    SPR_setPosition(pneu->sprite, pneu->x, pneu->y);
    SPR_setVisibility(pneu->sprite, VISIBLE);
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

void atualizarInimigosETiros(Inimigo* n64, Inimigo* aranha, Inimigo* disjuntor, Inimigo* pneu, Tiro tirosInimigo[], Player* player, u16 edicao) {
    // 1. N64 (sempre ativo)
 // 1. N64 (flutua suavemente para cima e para baixo)
 if (n64->ativo) {
     if (n64->timerTiro % 3 == 0) {
         n64->y += n64->vy;
         if (n64->y >= n64->baseY + 12) n64->vy = -1;
         else if (n64->y <= n64->baseY - 12) n64->vy = 1;
         SPR_setPosition(n64->sprite, n64->x, n64->y);
     }

     if (n64->timerTiro > 0) {
         n64->timerTiro--;
     } else {
         n64->timerTiro = 60;
         dispararTiroInimigo(n64, tirosInimigo, player->x, player->y);
     }
 }

 // 2. Aranha (balança suavemente para a direita e para a esquerda)
 if (edicao > 25) {
        if (aranha->ativo) {
            if (aranha->timerTiro % 2 == 0) {
                aranha->x += aranha->vx;
                if (aranha->x >= aranha->baseX + 20) aranha->vx = -1;
                else if (aranha->x <= aranha->baseX - 20) aranha->vx = 1;
                SPR_setPosition(aranha->sprite, aranha->x, aranha->y);
            }

            if (aranha->timerPisca > 0) {
                aranha->timerPisca--;
                SPR_setVisibility(aranha->sprite, (aranha->timerPisca % 4 < 2) ? HIDDEN : VISIBLE);
            } else {
                SPR_setVisibility(aranha->sprite, VISIBLE);
            }

            if (aranha->timerTiro > 0) {
                aranha->timerTiro--;
            } else {
                aranha->timerTiro = 75;
                dispararTiroInimigo(aranha, tirosInimigo, player->x, player->y);
            }
        }
            }
        } else {
            if (aranha->timerRespawn > 0) {
                aranha->timerRespawn--;
            } else {
                spawnAranha(aranha);
            }
        }
    }

    // 3. Disjuntor (a partir do 55 2JOIN — 5s antes)
    if (edicao >= 55) {
        if (disjuntor->ativo) {
            if (disjuntor->timerPisca > 0) {
                disjuntor->timerPisca--;
                SPR_setVisibility(disjuntor->sprite, (disjuntor->timerPisca % 4 < 2) ? HIDDEN : VISIBLE);
            } else {
                SPR_setVisibility(disjuntor->sprite, VISIBLE);
            }
        } else {
            if (disjuntor->timerRespawn > 0) {
                disjuntor->timerRespawn--;
            } else {
                spawnDisjuntor(disjuntor, player->x);
            }
        }
    }

    // 4. Pneu (a partir do 75 2JOIN — 5s antes)
    if (edicao >= 75) {
        if (pneu->ativo) {
            pneu->x += pneu->vx;
            pneu->y += pneu->vy;

            pneu->timerTiro++;
            if (pneu->timerTiro >= 5) {
                pneu->timerTiro = 0;
                pneu->vy++;
            }

            if (pneu->y >= 198) {
                pneu->y = 198;
                pneu->vy = -5;
            }

            if (pneu->x < -32) {
                pneu->ativo = FALSE;
                pneu->timerRespawn = 45;
                SPR_setVisibility(pneu->sprite, HIDDEN);
            } else {
                SPR_setPosition(pneu->sprite, pneu->x, pneu->y);

                // Encostou no jogador: Pneu morre sem dar pontos, jogador toma dano e perde o combo
                if (pneu->x + 28 > player->x + 16 && pneu->x + 4 < player->x + 32 &&
                    pneu->y + 20 > player->y + 16 && pneu->y + 4 < player->y + 32)
                {
                    pneu->ativo = FALSE;
                    pneu->timerRespawn = 45;
                    SPR_setVisibility(pneu->sprite, HIDDEN);
                    player->timerPiscaDano = 30;
                    tocarSomDanoPSG();
                    resetarCombo(player);
                }
            }
        } else {
            if (pneu->timerRespawn > 0) {
                pneu->timerRespawn--;
            } else {
                spawnPneu(pneu);
            }
        }
    }

    // 5. Tiros inimigos e colisão com o jogador
    for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
        if (tirosInimigo[i].ativo) {
            tirosInimigo[i].x += tirosInimigo[i].vx;
            tirosInimigo[i].y += tirosInimigo[i].vy;

            if (tirosInimigo[i].y > 224 || tirosInimigo[i].x < -16 || tirosInimigo[i].x > 320) {
                SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
                tirosInimigo[i].ativo = FALSE;
            } else {
                SPR_setPosition(tirosInimigo[i].sprite, tirosInimigo[i].x, tirosInimigo[i].y);

                if (tirosInimigo[i].x + 12 > player->x + 16 && tirosInimigo[i].x + 4 < player->x + 32 &&
                    tirosInimigo[i].y + 12 > player->y + 16 && tirosInimigo[i].y + 4 < player->y + 32)
                {
                    tirosInimigo[i].ativo = FALSE;
                    SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
                    player->timerPiscaDano = 30;
                    tocarSomDanoPSG();
                    resetarCombo(player);
                }
            }
        }
    }
}
