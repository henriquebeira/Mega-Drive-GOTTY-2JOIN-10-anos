#include "jogo.h"

void atualizarPlacar(u16 pontos) {
    char textoPontos[16];
    uintToStr(pontos, textoPontos, 1);
    VDP_drawText(textoPontos, 38 - strlen(textoPontos), 2);
}

void atualizarContadorEdicao(u16 edicao) {
    char textoEdicao[20];
    char numStr[8];
    uintToStr(edicao, numStr, 1);

    strcpy(textoEdicao, numStr);
    strcat(textoEdicao, " 2JOIN");

    VDP_drawText(textoEdicao, 2, 2);
}

int main(bool hard)
{
    JOY_init();
    SPR_init();

    XGM_setPCM(SFX_TIRO_ID, sfx_tiro, sizeof(sfx_tiro));
    XGM_setPCM(BGM_MENU_ID, bgm_menu, sizeof(bgm_menu));
    XGM_setPCM(BGM_JOGO_ID, bgm_jogo, sizeof(bgm_jogo));

    // --- TELA INICIAL (ESTÁTICA E NÍTIDA COM TEXTO PISCANDO) ---
    PAL_setPalette(PAL0, img_titulo.palette->data, CPU);
    VDP_drawImageEx(BG_B, &img_titulo, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX), 0, 0, FALSE, CPU);
    XGM_startPlayPCM(BGM_MENU_ID, 1, SOUND_PCM_CH2);

    u8 timerPisca = 0;
    bool mostraTexto = TRUE;

    while(TRUE)
    {
        random();

        if (!XGM_isPlayingPCM(SOUND_PCM_CH2_MSK)) {
            XGM_startPlayPCM(BGM_MENU_ID, 1, SOUND_PCM_CH2);
        }

        // Faz o "PRESS START 2 JOIN" piscar a cada 25 frames sem mexer na imagem de fundo
        timerPisca++;
        if (timerPisca >= 25) {
            timerPisca = 0;
            mostraTexto = !mostraTexto;

            if (mostraTexto) {
                VDP_drawText("PRESS START 2 JOIN", 11, 15);
            } else {
                VDP_clearText(11, 15, 18);
            }
        }

        if (JOY_readJoypad(JOY_1) & BUTTON_START) break;
        SYS_doVBlankProcess();
    }

    // Limpa a tela inicial antes de começar a fase
    XGM_stopPlayPCM(SOUND_PCM_CH2);
    VDP_clearPlane(BG_B, TRUE);
    VDP_clearTextArea(0, 0, 40, 28);
    XGM_startPlayPCM(BGM_JOGO_ID, 1, SOUND_PCM_CH2);

    // --- INÍCIO DA FASE ---
    u16 pontos = 0;
    u16 edicao = 1;
    u16 timerEdicao = 0;

    PAL_setPalette(PAL1, spr_idle.palette->data, DMA);
    PAL_setPalette(PAL2, spr_tiro.palette->data, DMA);
    PAL_setPalette(PAL3, spr_n64.palette->data, DMA);

    PAL_setPalette(PAL0, spr_tiro_inimigo_vertical.palette->data, DMA);
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x002266));
    PAL_setColor(15, RGB24_TO_VDPCOLOR(0xFFFFFF));

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
        tiros[i].sprite = SPR_addSprite(&spr_tiro, 0, -16, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
        SPR_setVisibility(tiros[i].sprite, HIDDEN);
    }

    Tiro tirosInimigo[MAX_TIROS_INIMIGO];
    for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
        tirosInimigo[i].ativo = FALSE;
        tirosInimigo[i].vx = 0;
        tirosInimigo[i].sprite = SPR_addSprite(&spr_tiro_inimigo_vertical, 0, -16, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
        SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
    }

    Inimigo inimigo;
    inimigo.timerRespawn = 0;
    inimigo.sprite = SPR_addSprite(&spr_n64, 0, -32, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
    spawnInimigo(&inimigo, edicao);

    while(TRUE)
    {
        if (!XGM_isPlayingPCM(SOUND_PCM_CH2_MSK)) {
            XGM_startPlayPCM(BGM_JOGO_ID, 1, SOUND_PCM_CH2);
        }

        // Incrementa o contador a cada 1 segundo (60 frames)
        timerEdicao++;
        if (timerEdicao >= 60) {
            timerEdicao = 0;
            edicao++;

            if (edicao >= 98) {
                break;
            }

            atualizarContadorEdicao(edicao);
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

        if (controle & (BUTTON_B | BUTTON_A)) {
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

        // Tiros do jogador e acerto no inimigo (N64 ou Aranha)
        for (u8 i = 0; i < MAX_TIROS; i++) {
            if (tiros[i].ativo) {
                tiros[i].y += tiros[i].vy;

                if (tiros[i].y < -16) {
                    SPR_setVisibility(tiros[i].sprite, HIDDEN);
                    tiros[i].ativo = FALSE;
                } else {
                    SPR_setPosition(tiros[i].sprite, tiros[i].x, tiros[i].y);

                    if (inimigo.ativo &&
                        tiros[i].x + 14 > inimigo.x && tiros[i].x + 2 < inimigo.x + inimigo.largura &&
                        tiros[i].y + 14 > inimigo.y && tiros[i].y + 2 < inimigo.y + inimigo.altura)
                    {
                        tiros[i].ativo = FALSE;
                        SPR_setVisibility(tiros[i].sprite, HIDDEN);

                        inimigo.ativo = FALSE;
                        inimigo.timerRespawn = 20;
                        SPR_setVisibility(inimigo.sprite, HIDDEN);

                        pontos++;
                        atualizarPlacar(pontos);
                    }
                }
            }
        }

        atualizarInimigoETiros(&inimigo, tirosInimigo, x, y, &timerPiscaDano, edicao);

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

    // --- TELA FINAL (CHEGOU NO 98) ---
    XGM_stopPlayPCM(SOUND_PCM_CH2);

    SPR_setVisibility(player, HIDDEN);
    SPR_setVisibility(inimigo.sprite, HIDDEN);
    for (u8 i = 0; i < MAX_TIROS; i++) {
        SPR_setVisibility(tiros[i].sprite, HIDDEN);
    }
    for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
        SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
    }
    SPR_update();

    VDP_clearTextArea(0, 0, 40, 28);
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x001133));

    char textoFinal[32];
    char numFinal[16];
    uintToStr(pontos, numFinal, 1);
    strcpy(textoFinal, "PONTUACAO FINAL: ");
    strcat(textoFinal, numFinal);

    VDP_drawText("98 2JOIN - FIM!", 12, 10);
    VDP_drawText(textoFinal, (40 - strlen(textoFinal)) / 2, 13);

    while(TRUE)
    {
        SYS_doVBlankProcess();
    }

    return 0;
}
