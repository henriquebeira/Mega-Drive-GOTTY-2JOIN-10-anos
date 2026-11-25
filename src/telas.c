#include "jogo.h"

void atualizarPlacar(u16 pontos) {
    char textoPontos[12];
    uintToStr(pontos, textoPontos, 1);
    VDP_drawText(textoPontos, 38 - strlen(textoPontos), 2);
}

void atualizarContadorEdicao(u16 edicao) {
    char textoEdicao[16];
    char numStr[8];
    uintToStr(edicao, numStr, 1);

    strcpy(textoEdicao, numStr);
    strcat(textoEdicao, " 2JOIN");

    VDP_clearText(2, 2, 10);
    VDP_drawText(textoEdicao, 2, 2);
}

void executarTelaInicial(void) {
    PAL_setPalette(PAL0, img_titulo.palette->data, CPU);
    PAL_setColor(31, RGB24_TO_VDPCOLOR(0xFFFFFF));
    VDP_setTextPalette(PAL1);
    VDP_drawImageEx(BG_B, &img_titulo, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX), 0, 0, FALSE, CPU);
    XGM_startPlayPCM(BGM_MENU_ID, 1, SOUND_PCM_CH2);

    u8 timerPisca = 0;
    bool mostraTexto = TRUE;
    VDP_drawText("PRESS START 2 JOIN", 11, 15);

    // Garante que o jogador soltou o START antes de aceitar um novo clique
    while (JOY_readJoypad(JOY_1) & BUTTON_START) {
        SYS_doVBlankProcess();
    }

    while(TRUE)
    {
        random();

        if (!XGM_isPlayingPCM(SOUND_PCM_CH2_MSK)) {
            XGM_startPlayPCM(BGM_MENU_ID, 1, SOUND_PCM_CH2);
        }

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

    // Limpa a tela inicial antes de iniciar a fase
    XGM_stopPlayPCM(SOUND_PCM_CH2);
    VDP_clearPlane(BG_B, TRUE);
    VDP_clearTextArea(0, 0, 40, 28);
    VDP_setTextPalette(PAL0);
}

void executarTelaFinal(u16 pontos) {
    XGM_stopPlayPCM(SOUND_PCM_CH2);

    SPR_reset();
    SPR_update();

    VDP_clearTextArea(0, 0, 40, 28);
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x001133));

    const char* tituloFim = "98 2JOIN - FIM DE JOGO!";
    char textoFinal[32];
    char numFinal[8];
    uintToStr(pontos, numFinal, 1);
    strcpy(textoFinal, "PONTUACAO FINAL: ");
    strcat(textoFinal, numFinal);
    const char* textoRestart = "PRESS START TO RESTART";

    VDP_drawText(tituloFim, (40 - strlen(tituloFim)) / 2, 10);
    VDP_drawText(textoFinal, (40 - strlen(textoFinal)) / 2, 13);
    VDP_drawText(textoRestart, (40 - strlen(textoRestart)) / 2, 17);

    while(TRUE)
    {
        if (JOY_readJoypad(JOY_1) & BUTTON_START) break;
        SYS_doVBlankProcess();
    }

    VDP_clearTextArea(0, 0, 40, 28);
}
