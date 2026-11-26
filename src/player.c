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
    p->spriteToasty = SPR_addSprite(&spr_toasty, 240, 224, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
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

    if (p->x < 0) p->x = 0;
    if (p->x > 288) p->x = 288;

    if (movendoAgora && !p->andando) {
        SPR_setDefinition(p->sprite, &spr_walk);
        p->andando = TRUE;
    } else if (!movendoAgora && p->andando) {
        SPR_setDefinition(p->sprite, &spr_idle);
        p->andando = FALSE;
    }

    SPR_setPosition(p->sprite, p->x, p->y);

    if (p->cooldownTiro > 0) p->cooldownTiro--;

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

    // Move os tiros ativos do jogador
    for (u8 i = 0; i < MAX_TIROS; i++) {
        if (tiros[i].ativo) {
            tiros[i].y += tiros[i].vy;

            if (tiros[i].y < -16) {
                SPR_setVisibility(tiros[i].sprite, HIDDEN);
                tiros[i].ativo = FALSE;
            } else {
                SPR_setPosition(tiros[i].sprite, tiros[i].x, tiros[i].y);
            }
        }
    }

    // Invencibilidade temporária
    if (p->timerPiscaDano > 0) {
        p->timerPiscaDano--;
        SPR_setVisibility(p->sprite, (p->timerPiscaDano % 4 < 2) ? HIDDEN : VISIBLE);
    } else {
        SPR_setVisibility(p->sprite, VISIBLE);
    }

    // Verifica todas as colisões em src/colisoes.c
    verificarColisoesPlayerETiros(p, tiros, n64, aranha, disjuntor, pneu, estilhacos, pontos);

    // Animação do Toasty no canto inferior direito
    if (p->timerToasty > 0) {
        p->timerToasty--;
        if (p->timerToasty > 45) {
            s16 posY = 224 - ((60 - p->timerToasty) * 3);
            if (posY < 176) posY = 176;
            SPR_setPosition(p->spriteToasty, 240, posY);
            SPR_setVisibility(p->spriteToasty, VISIBLE);
        } else if (p->timerToasty > 15) {
            SPR_setPosition(p->spriteToasty, 240, 176);
            SPR_setVisibility(p->spriteToasty, VISIBLE);
        } else {
            s16 posY = 176 + ((15 - p->timerToasty) * 3);
            SPR_setPosition(p->spriteToasty, 240, posY);
        }

        if (p->timerToasty == 0) {
            SPR_setVisibility(p->spriteToasty, HIDDEN);
        }
    }
}
