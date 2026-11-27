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

static u16 timerOnda = 0;

void inicializarInimigos(Inimigo n64[], Inimigo aranha[], Inimigo disjuntor[], Inimigo* pneu, Tiro tirosInimigo[]) {
    timerOnda = 90;

    for (u8 i = 0; i < MAX_N64; i++) {
        n64[i].sprite = SPR_addSprite(&spr_n64, 0, -40, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
        n64[i].ativo = FALSE;
        n64[i].timerRespawn = 0;
        SPR_setVisibility(n64[i].sprite, HIDDEN);
    }

    for (u8 i = 0; i < MAX_ARANHA; i++) {
        aranha[i].sprite = SPR_addSprite(&spr_aranha, 0, -40, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));
        aranha[i].ativo = FALSE;
        aranha[i].timerRespawn = 0;
        SPR_setVisibility(aranha[i].sprite, HIDDEN);
    }

    for (u8 i = 0; i < MAX_DISJUNTOR; i++) {
        disjuntor[i].sprite = SPR_addSprite(&spr_disjuntor, 0, -40, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
        disjuntor[i].ativo = FALSE;
        disjuntor[i].timerRespawn = 0;
        SPR_setVisibility(disjuntor[i].sprite, HIDDEN);
    }

    pneu->sprite = SPR_addSprite(&spr_pneu, 0, -40, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    pneu->ativo = FALSE;
    pneu->timerRespawn = 600; // 10 segundos para o primeiro Pneu
    SPR_setVisibility(pneu->sprite, HIDDEN);

    for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
        tirosInimigo[i].sprite = SPR_addSprite(&spr_tiros, 0, -16, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
        tirosInimigo[i].ativo = FALSE;
        SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
    }

    // Começa com 1 N64 na tela (0..15s)
    spawnN64(&n64[0]);
}

void atualizarInimigosETiros(Inimigo n64[], Inimigo aranha[], Inimigo disjuntor[], Inimigo* pneu, Tiro tirosInimigo[], Player* player, u16 edicao) {
    // Define limites simultâneos por onda (1 a menos que no Construct 3)
    u8 limN64 = 1;
    u8 limAranha = 0;
    u8 limDisjuntor = 0;

    if (edicao > 60) {
        limN64 = 3; limAranha = 2; limDisjuntor = 1;
    } else if (edicao > 45) {
        limN64 = 2; limAranha = 1; limDisjuntor = 1;
    } else if (edicao > 30) {
        limN64 = 2; limAranha = 1; limDisjuntor = 0;
    } else if (edicao > 15) {
        limN64 = 1; limAranha = 1; limDisjuntor = 0;
    }

    // A cada 1,5s (90 frames), cria 1 inimigo faltante até atingir o limite da onda (enquanto edicao < 98)
    if (edicao < 98) {
        if (timerOnda > 0) {
            timerOnda--;
        } else {
            timerOnda = 90;
            u8 ativosN64 = 0, ativosAranha = 0, ativosDisj = 0;
            for (u8 i = 0; i < MAX_N64; i++) if (n64[i].ativo) ativosN64++;
            for (u8 i = 0; i < MAX_ARANHA; i++) if (aranha[i].ativo) ativosAranha++;
            for (u8 i = 0; i < MAX_DISJUNTOR; i++) if (disjuntor[i].ativo) ativosDisj++;

            if (ativosN64 < limN64) {
                for (u8 i = 0; i < MAX_N64; i++) {
                    if (!n64[i].ativo) { spawnN64(&n64[i]); break; }
                }
            }
            if (ativosAranha < limAranha) {
                for (u8 i = 0; i < MAX_ARANHA; i++) {
                    if (!aranha[i].ativo) { spawnAranha(&aranha[i]); break; }
                }
            }
            if (ativosDisj < limDisjuntor) {
                for (u8 i = 0; i < MAX_DISJUNTOR; i++) {
                    if (!disjuntor[i].ativo) { spawnDisjuntor(&disjuntor[i], player->x); break; }
                }
            }
        }
    }

    // 1. Atualiza todos os N64 ativos
    for (u8 i = 0; i < MAX_N64; i++) {
        if (n64[i].ativo) {
            if (n64[i].timerTiro % 3 == 0) {
                n64[i].y += n64[i].vy;
                if (n64[i].y >= n64[i].baseY + 12) n64[i].vy = -1;
                else if (n64[i].y <= n64[i].baseY - 12) n64[i].vy = 1;
                SPR_setPosition(n64[i].sprite, n64[i].x, n64[i].y);
            }
            if (n64[i].timerTiro > 0) {
                n64[i].timerTiro--;
            } else {
                n64[i].timerTiro = 60;
                dispararTiroInimigo(&n64[i], tirosInimigo, player->x, player->y);
            }
        }
    }

    // 2. Atualiza todas as Aranhas ativas
    for (u8 i = 0; i < MAX_ARANHA; i++) {
        if (aranha[i].ativo) {
            if (aranha[i].timerTiro % 2 == 0) {
                aranha[i].x += aranha[i].vx;
                if (aranha[i].x >= aranha[i].baseX + 20) aranha[i].vx = -1;
                else if (aranha[i].x <= aranha[i].baseX - 20) aranha[i].vx = 1;
                SPR_setPosition(aranha[i].sprite, aranha[i].x, aranha[i].y);
            }
            if (aranha[i].timerPisca > 0) {
                aranha[i].timerPisca--;
                SPR_setVisibility(aranha[i].sprite, (aranha[i].timerPisca % 4 < 2) ? HIDDEN : VISIBLE);
            } else {
                SPR_setVisibility(aranha[i].sprite, VISIBLE);
            }
            if (aranha[i].timerTiro > 0) {
                aranha[i].timerTiro--;
            } else {
                aranha[i].timerTiro = 75;
                dispararTiroInimigo(&aranha[i], tirosInimigo, player->x, player->y);
            }
        }
    }

    // 3. Atualiza Disjuntor ativo (sem atirar)
    for (u8 i = 0; i < MAX_DISJUNTOR; i++) {
        if (disjuntor[i].ativo) {
            if (disjuntor[i].timerPisca > 0) {
                disjuntor[i].timerPisca--;
                SPR_setVisibility(disjuntor[i].sprite, (disjuntor[i].timerPisca % 4 < 2) ? HIDDEN : VISIBLE);
            } else {
                SPR_setVisibility(disjuntor[i].sprite, VISIBLE);
            }
        }
    }

    // 4. Pneu a cada 10 segundos (600 frames) enquanto edicao < 98
    if (pneu->ativo) {
        pneu->timerTiro++;
        pneu->x += pneu->vx;
        pneu->y += pneu->vy;

        if (pneu->x <= 0) {
            pneu->x = 0;
            pneu->vx = 2;
        } else if (pneu->x >= 288) {
            pneu->x = 288;
            pneu->vx = -2;
        }

        // Quica exatamente na linha dos pés do jogador (y >= 190)
        if (pneu->y >= 190) {
            pneu->y = 190;
            pneu->vy = -7;
        } else if (pneu->timerTiro % 3 == 0) {
            pneu->vy++;
        }

        SPR_setPosition(pneu->sprite, pneu->x, pneu->y);
    } else if (edicao < 98) {
        if (pneu->timerRespawn > 0) {
            pneu->timerRespawn--;
        } else {
            pneu->timerRespawn = 600;
            spawnPneu(pneu);
        }
    }

    // 5. Atualiza os tiros dos inimigos
    for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
        if (tirosInimigo[i].ativo) {
            tirosInimigo[i].x += tirosInimigo[i].vx;
            tirosInimigo[i].y += tirosInimigo[i].vy;

            if (tirosInimigo[i].x < -16 || tirosInimigo[i].x > 320 || tirosInimigo[i].y < -16 || tirosInimigo[i].y > 224) {
                tirosInimigo[i].ativo = FALSE;
                SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
            } else {
                SPR_setPosition(tirosInimigo[i].sprite, tirosInimigo[i].x, tirosInimigo[i].y);
                if (player->timerPiscaDano == 0 &&
                    tirosInimigo[i].x + 12 > player->x + 8 && tirosInimigo[i].x + 4 < player->x + 24 &&
                    tirosInimigo[i].y + 12 > player->y + 8 && tirosInimigo[i].y + 4 < player->y + 32) {
                    tirosInimigo[i].ativo = FALSE;
                    SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
                    player->timerPiscaDano = 60;
                    tocarSomDanoPSG();
                    resetarCombo(player);
                }
            }
        }
    }
}
