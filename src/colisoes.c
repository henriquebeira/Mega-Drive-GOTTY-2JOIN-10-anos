#include "jogo.h"

void verificarColisoesPlayerETiros(Player* p, Tiro tiros[], Inimigo n64[], Inimigo aranha[], Inimigo disjuntor[], Inimigo* pneu, Estilhaco estilhacos[], u32* pontos) {
    // Tiros do jogador contra os inimigos
    for (u8 i = 0; i < MAX_TIROS; i++) {
        if (!tiros[i].ativo) continue;

        // 1. N64 (1 ponto x multiplicador)
        for (u8 j = 0; j < MAX_N64; j++) {
            if (tiros[i].ativo && n64[j].ativo &&
                tiros[i].x + 14 > n64[j].x && tiros[i].x + 2 < n64[j].x + n64[j].largura &&
                tiros[i].y + 14 > n64[j].y && tiros[i].y + 2 < n64[j].y + n64[j].altura) {

                tiros[i].ativo = FALSE;
                SPR_setVisibility(tiros[i].sprite, HIDDEN);

                n64[j].ativo = FALSE;
                SPR_setVisibility(n64[j].sprite, HIDDEN);
                dispararEstilhacos(estilhacos, &spr_n64, PAL3, n64[j].x + 12, n64[j].y + 12);
                XGM_startPlayPCM(SFX_EXPLOSAO_ID, 12, SOUND_PCM_CH4);
                *pontos += (1 * p->multiplicador);
                atualizarPlacar(*pontos);
                registrarAbateCombo(p);
                break;
            }
        }

        // 2. Aranha (2 pontos x multiplicador)
        for (u8 j = 0; j < MAX_ARANHA; j++) {
            if (tiros[i].ativo && aranha[j].ativo &&
                tiros[i].x + 14 > aranha[j].x && tiros[i].x + 2 < aranha[j].x + aranha[j].largura &&
                tiros[i].y + 14 > aranha[j].y && tiros[i].y + 2 < aranha[j].y + aranha[j].altura) {

                tiros[i].ativo = FALSE;
                SPR_setVisibility(tiros[i].sprite, HIDDEN);

                aranha[j].hp--;
                if (aranha[j].hp == 0) {
                    aranha[j].ativo = FALSE;
                    SPR_setVisibility(aranha[j].sprite, HIDDEN);
                    dispararEstilhacos(estilhacos, &spr_aranha, PAL3, aranha[j].x + 16, aranha[j].y + 16);
                    XGM_startPlayPCM(SFX_EXPLOSAO_ID, 12, SOUND_PCM_CH4);
                    *pontos += (2 * p->multiplicador);
                    atualizarPlacar(*pontos);
                    registrarAbateCombo(p);
                } else {
                    aranha[j].timerPisca = 12;
                }
                break;
            }
        }

        // 3. Disjuntor (3 pontos x multiplicador)
        for (u8 j = 0; j < MAX_DISJUNTOR; j++) {
            if (tiros[i].ativo && disjuntor[j].ativo &&
                tiros[i].x + 14 > disjuntor[j].x && tiros[i].x + 2 < disjuntor[j].x + disjuntor[j].largura &&
                tiros[i].y + 14 > disjuntor[j].y && tiros[i].y + 2 < disjuntor[j].y + disjuntor[j].altura) {

                tiros[i].ativo = FALSE;
                SPR_setVisibility(tiros[i].sprite, HIDDEN);

                disjuntor[j].hp--;
                if (disjuntor[j].hp == 0) {
                    disjuntor[j].ativo = FALSE;
                    SPR_setVisibility(disjuntor[j].sprite, HIDDEN);
                    dispararEstilhacos(estilhacos, &spr_disjuntor, PAL3, disjuntor[j].x + 24, disjuntor[j].y + 16);
                    XGM_startPlayPCM(SFX_EXPLOSAO_ID, 12, SOUND_PCM_CH4);
                    *pontos += (3 * p->multiplicador);
                    atualizarPlacar(*pontos);
                    registrarAbateCombo(p);
                } else {
                    disjuntor[j].timerPisca = 12;
                }
                break;
            }
        }

        // 4. Pneu (5 pontos x multiplicador)
        if (tiros[i].ativo && pneu->ativo &&
            tiros[i].x + 14 > pneu->x && tiros[i].x + 2 < pneu->x + pneu->largura &&
            tiros[i].y + 14 > pneu->y && tiros[i].y + 2 < pneu->y + pneu->altura) {

            tiros[i].ativo = FALSE;
            SPR_setVisibility(tiros[i].sprite, HIDDEN);

            pneu->ativo = FALSE;
            SPR_setVisibility(pneu->sprite, HIDDEN);
            dispararEstilhacos(estilhacos, &spr_pneu, PAL1, pneu->x + 16, pneu->y + 16);
            XGM_startPlayPCM(SFX_EXPLOSAO_ID, 12, SOUND_PCM_CH4);
            *pontos += (5 * p->multiplicador);
            atualizarPlacar(*pontos);
            registrarAbateCombo(p);
        }
    }

    // Colisão direta do corpo do jogador com os inimigos
    if (p->timerPiscaDano == 0) {
        bool tomouDano = FALSE;

        for (u8 j = 0; j < MAX_N64; j++) {
            if (n64[j].ativo &&
                p->x + 24 > n64[j].x && p->x + 8 < n64[j].x + n64[j].largura &&
                p->y + 32 > n64[j].y && p->y + 8 < n64[j].y + n64[j].altura) {
                tomouDano = TRUE;
            }
        }

        for (u8 j = 0; j < MAX_ARANHA; j++) {
            if (aranha[j].ativo &&
                p->x + 24 > aranha[j].x && p->x + 8 < aranha[j].x + aranha[j].largura &&
                p->y + 32 > aranha[j].y && p->y + 8 < aranha[j].y + aranha[j].altura) {
                tomouDano = TRUE;
            }
        }

        for (u8 j = 0; j < MAX_DISJUNTOR; j++) {
            if (disjuntor[j].ativo &&
                p->x + 24 > disjuntor[j].x && p->x + 8 < disjuntor[j].x + disjuntor[j].largura &&
                p->y + 32 > disjuntor[j].y && p->y + 8 < disjuntor[j].y + disjuntor[j].altura) {
                tomouDano = TRUE;
            }
        }

        if (pneu->ativo &&
            p->x + 24 > pneu->x && p->x + 8 < pneu->x + pneu->largura &&
            p->y + 32 > pneu->y && p->y + 8 < pneu->y + pneu->altura) {
            tomouDano = TRUE;
        }

        if (tomouDano) {
            p->timerPiscaDano = 60;
            tocarSomDanoPSG();
            resetarCombo(p);
        }
    }
}
