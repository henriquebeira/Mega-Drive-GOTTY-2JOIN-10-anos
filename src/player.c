#include "jogo.h"

void inicializarPlayer(Player* p, Tiro tiros[]) {
    p->x = 136;
    p->y = 174;
    p->andando = FALSE;
    p->olhandoDireita = TRUE;
    p->cooldownTiro = 0;
    p->timerPiscaDano = 0;
    p->sprite = SPR_addSprite(&spr_idle, p->x, p->y, TILE_ATTR(PAL1, FALSE, FALSE, TRUE));

    for (u8 i = 0; i < MAX_TIROS; i++) {
        tiros[i].ativo = FALSE;
        tiros[i].vx = 0;
        tiros[i].sprite = SPR_addSprite(&spr_tiros, 0, -16, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
        SPR_setVisibility(tiros[i].sprite, HIDDEN);
    }

    p->multiplicador = 1;
    p->killsNoNivel = 0;
    p->metaProximoNivel = 4;  
    p->spriteMult = SPR_addSprite(&spr_mult, 288, 44, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
    SPR_setVisibility(p->spriteMult, HIDDEN);
    
    p->timerToasty = 0;
    p->spriteToasty = SPR_addSprite(&spr_toasty, 248, 224, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
    SPR_setVisibility(p->spriteToasty, HIDDEN);

}

void atualizarPlayerETiros(Player* p, Tiro tiros[], Inimigo n64[], Inimigo aranha[], Inimigo disjuntor[], Inimigo* pneu, Estilhaco estilhacos[], u32* pontos) {

    u16 controle = JOY_readJoypad(JOY_1);
    bool movendoAgora = FALSE;
    bool segurandoTiro = (controle & (BUTTON_A | BUTTON_B | BUTTON_C)) != 0;
    s16 velocidadePlayer = segurandoTiro ? 1 : 2;

    if (controle & BUTTON_LEFT) {
        p->x -= velocidadePlayer;
        SPR_setHFlip(p->sprite, FALSE);
        p->olhandoDireita = FALSE;
        movendoAgora = TRUE;
    }
    if (controle & BUTTON_RIGHT) {
        p->x += velocidadePlayer;
        SPR_setHFlip(p->sprite, TRUE);
        p->olhandoDireita = TRUE;
        movendoAgora = TRUE;
    }

    if (movendoAgora && !p->andando) {
        PAL_setPalette(PAL1, spr_run.palette->data, DMA);
        SPR_setDefinition(p->sprite, &spr_run);
        p->andando = TRUE;
    } else if (!movendoAgora && p->andando) {
        PAL_setPalette(PAL1, spr_idle.palette->data, DMA);
        SPR_setDefinition(p->sprite, &spr_idle);
        p->andando = FALSE;
    }

    // Autofire
    if (p->cooldownTiro > 0) {
        p->cooldownTiro--;
    }

    if (segurandoTiro && p->cooldownTiro == 0) {
        for (u8 i = 0; i < MAX_TIROS; i++) {
            if (!tiros[i].ativo) {
                tiros[i].ativo = TRUE;
                tiros[i].y = p->y - 2;
                tiros[i].vy = -5;
                tiros[i].x = p->olhandoDireita ? (p->x + 8) : (p->x + 24);

                SPR_setPosition(tiros[i].sprite, tiros[i].x, tiros[i].y);
                SPR_setVisibility(tiros[i].sprite, VISIBLE);
                XGM_startPlayPCM(SFX_TIRO_ID, 10, SOUND_PCM_CH1);
                p->cooldownTiro = 12;
                break;
            }
        }
    }

    // Tiros do jogador e colisão com os 4 inimigos
    for (u8 i = 0; i < MAX_TIROS; i++) {
        if (tiros[i].ativo) {
            tiros[i].y += tiros[i].vy;

            if (tiros[i].y < -16) {
                SPR_setVisibility(tiros[i].sprite, HIDDEN);
                tiros[i].ativo = FALSE;
            } else {
                SPR_setPosition(tiros[i].sprite, tiros[i].x, tiros[i].y);

                // 1. N64 (1 ponto x multiplicador)
                if (n64->ativo &&
                    tiros[i].x + 14 > n64->x && tiros[i].x + 2 < n64->x + n64->largura &&
                    tiros[i].y + 14 > n64->y && tiros[i].y + 2 < n64->y + n64->altura)
                {
                    tiros[i].ativo = FALSE;
                    SPR_setVisibility(tiros[i].sprite, HIDDEN);

                    n64->ativo = FALSE;
                    n64->timerRespawn = 20;
                    SPR_setVisibility(n64->sprite, HIDDEN);

                    *pontos += 1 * p->multiplicador;
                    atualizarPlacar(*pontos);
                    registrarAbateCombo(p);
                    XGM_startPlayPCM(SFX_EXPLOSAO_ID, 11, SOUND_PCM_CH4);
                    dispararEstilhacos(estilhacos, &spr_n64, PAL3, n64->x + 12, n64->y + 12);
                }
                // 2. Aranha (2 pontos x multiplicador)
                else if (aranha->ativo &&
                    tiros[i].x + 14 > aranha->x && tiros[i].x + 2 < aranha->x + aranha->largura &&
                    tiros[i].y + 14 > aranha->y && tiros[i].y + 2 < aranha->y + aranha->altura)
                {
                    tiros[i].ativo = FALSE;
                    SPR_setVisibility(tiros[i].sprite, HIDDEN);

                    if (aranha->hp > 1) {
                        aranha->hp--;
                        aranha->timerPisca = 16;
                    } else {
                        aranha->ativo = FALSE;
                        aranha->timerPisca = 0;
                        aranha->timerRespawn = 30;
                        SPR_setVisibility(aranha->sprite, HIDDEN);

                        *pontos += 2 * p->multiplicador;
                        atualizarPlacar(*pontos);
                        registrarAbateCombo(p);
                        XGM_startPlayPCM(SFX_EXPLOSAO_ID, 11, SOUND_PCM_CH4);
                        dispararEstilhacos(estilhacos, &spr_aranha, PAL2, aranha->x + 16, aranha->y + 16);
                    }
                }
                // 3. Disjuntor (3 pontos x multiplicador)
                else if (disjuntor->ativo &&
                    tiros[i].x + 14 > disjuntor->x && tiros[i].x + 2 < disjuntor->x + disjuntor->largura &&
                    tiros[i].y + 14 > disjuntor->y && tiros[i].y + 2 < disjuntor->y + disjuntor->altura)
                {
                    tiros[i].ativo = FALSE;
                    SPR_setVisibility(tiros[i].sprite, HIDDEN);

                    if (disjuntor->hp > 1) {
                        disjuntor->hp--;
                        disjuntor->timerPisca = 16;
                    } else {
                        disjuntor->ativo = FALSE;
                        disjuntor->timerPisca = 0;
                        disjuntor->timerRespawn = 45;
                        SPR_setVisibility(disjuntor->sprite, HIDDEN);

                        *pontos += 3 * p->multiplicador;
                        atualizarPlacar(*pontos);
                        registrarAbateCombo(p);
                        XGM_startPlayPCM(SFX_EXPLOSAO_ID, 11, SOUND_PCM_CH4);
                        dispararEstilhacos(estilhacos, &spr_disjuntor, PAL3, disjuntor->x + 24, disjuntor->y + 16);
                    }
                }
                // 4. Pneu (1 ponto x multiplicador)
                else if (pneu->ativo &&
                    tiros[i].x + 14 > pneu->x && tiros[i].x + 2 < pneu->x + pneu->largura &&
                    tiros[i].y + 14 > pneu->y && tiros[i].y + 2 < pneu->y + pneu->altura)
                {
                    tiros[i].ativo = FALSE;
                    SPR_setVisibility(tiros[i].sprite, HIDDEN);

                    pneu->ativo = FALSE;
                    pneu->timerPisca = 0;
                    pneu->timerRespawn = 45;
                    SPR_setVisibility(pneu->sprite, HIDDEN);

                    *pontos += 1 * p->multiplicador;
                    atualizarPlacar(*pontos);
                    registrarAbateCombo(p);
                    XGM_startPlayPCM(SFX_EXPLOSAO_ID, 11, SOUND_PCM_CH4);
                    dispararEstilhacos(estilhacos, &spr_pneu, PAL1, pneu->x + 16, pneu->y + 12);
                }
            }
        }
    }

    // Piscada de dano e limites horizontais
    if (p->timerPiscaDano > 0) {
        p->timerPiscaDano--;
        SPR_setVisibility(p->sprite, (p->timerPiscaDano % 4 < 2) ? HIDDEN : VISIBLE);
    } else {
        SPR_setVisibility(p->sprite, VISIBLE);
    }

    if (p->x < 0)   p->x = 0;
    if (p->x > 272) p->x = 272;

    SPR_setPosition(p->sprite, p->x, p->y);
    atualizarAnimacaoToasty(p);
}
