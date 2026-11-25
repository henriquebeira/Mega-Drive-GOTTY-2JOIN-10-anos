#include "jogo.h"

int main(bool hard)
{
    JOY_init();
    SPR_init();

    XGM_setPCM(SFX_TIRO_ID, sfx_tiro, sizeof(sfx_tiro));
    XGM_setPCM(BGM_MENU_ID, bgm_menu, sizeof(bgm_menu));
    XGM_setPCM(BGM_JOGO_ID, bgm_jogo, sizeof(bgm_jogo));

    while(TRUE)
    {
        executarTelaInicial();

        // --- INÍCIO DA FASE ---
        XGM_startPlayPCM(BGM_JOGO_ID, 1, SOUND_PCM_CH2);

        u16 pontos = 0;
        u16 edicao = 1;
        u16 timerEdicao = 0;

        PAL_setPalette(PAL1, spr_idle.palette->data, DMA); // Jogador + Tiros azuis do jogador
        PAL_setPalette(PAL2, spr_aranha.palette->data, DMA); // Aranha
        PAL_setPalette(PAL3, spr_n64.palette->data, DMA);    // N64

        PAL_setPalette(PAL0, spr_tiro_inimigo_vertical.palette->data, DMA); // Tiros de fogo dos inimigos
        PAL_setColor(0, RGB24_TO_VDPCOLOR(0x002266));  // Fundo azul escuro
        PAL_setColor(15, RGB24_TO_VDPCOLOR(0xFFFFFF)); // Textos em branco

        atualizarContadorEdicao(edicao);
        atualizarPlacar(pontos);

        s16 x = 136;
        s16 y = 174;
        bool andando = FALSE;
        bool olhandoDireita = TRUE;
        bool botaoTiroTravado = FALSE;
        u16 timerPiscaDano = 0;

        Sprite* player = SPR_addSprite(&spr_idle, x, y, TILE_ATTR(PAL1, FALSE, FALSE, TRUE));

        Tiro tiros[MAX_TIROS];
        for (u8 i = 0; i < MAX_TIROS; i++) {
            tiros[i].ativo = FALSE;
            tiros[i].vx = 0;
            tiros[i].sprite = SPR_addSprite(&spr_tiro, 0, -16, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
            SPR_setVisibility(tiros[i].sprite, HIDDEN);
        }

        Tiro tirosInimigo[MAX_TIROS_INIMIGO];
        for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
            tirosInimigo[i].ativo = FALSE;
            tirosInimigo[i].vx = 0;
            tirosInimigo[i].sprite = SPR_addSprite(&spr_tiro_inimigo_vertical, 0, -16, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
            SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
        }

        Inimigo n64;
        n64.timerRespawn = 0;
        n64.sprite = SPR_addSprite(&spr_n64, 0, -32, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
        spawnN64(&n64);

        // Aranha agora usa a PAL2 com todas as suas cores originais!
        Inimigo aranha;
        aranha.ativo = FALSE;
        aranha.timerRespawn = 0;
        aranha.sprite = SPR_addSprite(&spr_aranha, 0, -32, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
        SPR_setVisibility(aranha.sprite, HIDDEN);

        while(TRUE)
        {
            if (!XGM_isPlayingPCM(SOUND_PCM_CH2_MSK)) {
                break;
            }

            timerEdicao++;
            if (timerEdicao >= 60) {
                timerEdicao = 0;
                edicao++;
                atualizarContadorEdicao(edicao);

                if (edicao >= 98) {
                    break;
                }
            }

            u16 controle = JOY_readJoypad(JOY_1);
            bool movendoAgora = FALSE;

            if (controle & BUTTON_LEFT) {
                x -= 2;
                SPR_setHFlip(player, FALSE);
                olhandoDireita = FALSE;
                movendoAgora = TRUE;
            }
            if (controle & BUTTON_RIGHT) {
                x += 2;
                SPR_setHFlip(player, TRUE);
                olhandoDireita = TRUE;
                movendoAgora = TRUE;
            }

            if (movendoAgora && !andando) {
                PAL_setPalette(PAL1, spr_run.palette->data, DMA);
                SPR_setDefinition(player, &spr_run);
                andando = TRUE;
            } else if (!movendoAgora && andando) {
                PAL_setPalette(PAL1, spr_idle.palette->data, DMA);
                SPR_setDefinition(player, &spr_idle);
                andando = FALSE;
            }

            if (controle & (BUTTON_A | BUTTON_B | BUTTON_C)) {
                if (!botaoTiroTravado) {
                    botaoTiroTravado = TRUE;

                    for (u8 i = 0; i < MAX_TIROS; i++) {
                        if (!tiros[i].ativo) {
                            tiros[i].ativo = TRUE;
                            tiros[i].y = y - 2;
                            tiros[i].vy = -5;
                            tiros[i].x = olhandoDireita ? (x + 8) : (x + 24);

                            SPR_setPosition(tiros[i].sprite, tiros[i].x, tiros[i].y);
                            SPR_setVisibility(tiros[i].sprite, VISIBLE);
                            XGM_startPlayPCM(SFX_TIRO_ID, 10, SOUND_PCM_CH1);
                            break;
                        }
                    }
                }
            } else {
                botaoTiroTravado = FALSE;
            }

            // Tiros do jogador e acerto no N64 ou na Aranha
            for (u8 i = 0; i < MAX_TIROS; i++) {
                if (tiros[i].ativo) {
                    tiros[i].y += tiros[i].vy;

                    if (tiros[i].y < -16) {
                        SPR_setVisibility(tiros[i].sprite, HIDDEN);
                        tiros[i].ativo = FALSE;
                    } else {
                        SPR_setPosition(tiros[i].sprite, tiros[i].x, tiros[i].y);

                        if (n64.ativo &&
                            tiros[i].x + 14 > n64.x && tiros[i].x + 2 < n64.x + n64.largura &&
                            tiros[i].y + 14 > n64.y && tiros[i].y + 2 < n64.y + n64.altura)
                        {
                            tiros[i].ativo = FALSE;
                            SPR_setVisibility(tiros[i].sprite, HIDDEN);

                            n64.ativo = FALSE;
                            n64.timerRespawn = 20;
                            SPR_setVisibility(n64.sprite, HIDDEN);

                            pontos++;
                            atualizarPlacar(pontos);
                        }
                        else if (aranha.ativo &&
                            tiros[i].x + 14 > aranha.x && tiros[i].x + 2 < aranha.x + aranha.largura &&
                            tiros[i].y + 14 > aranha.y && tiros[i].y + 2 < aranha.y + aranha.altura)
                        {
                            tiros[i].ativo = FALSE;
                            SPR_setVisibility(tiros[i].sprite, HIDDEN);

                            aranha.ativo = FALSE;
                            aranha.timerRespawn = 30;
                            SPR_setVisibility(aranha.sprite, HIDDEN);

                            pontos++;
                            atualizarPlacar(pontos);
                        }
                    }
                }
            }

            atualizarInimigosETiros(&n64, &aranha, tirosInimigo, x, y, &timerPiscaDano, edicao);

            if (timerPiscaDano > 0) {
                timerPiscaDano--;
                SPR_setVisibility(player, (timerPiscaDano % 4 < 2) ? HIDDEN : VISIBLE);
            } else {
                SPR_setVisibility(player, VISIBLE);
            }

            if (x < 0)   x = 0;
            if (x > 272) x = 272;

            SPR_setPosition(player, x, y);
            SPR_update();

            SYS_doVBlankProcess();
        }

        // Aguarda a música terminar com a tela congelada no 98 2JOIN
        while (XGM_isPlayingPCM(SOUND_PCM_CH2_MSK)) {
            SYS_doVBlankProcess();
        }

        executarTelaFinal(pontos);
    }

    return 0;
}
