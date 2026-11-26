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

// Tiles sólidos coloridos para desenhar a barra vertical no BG_A
static const u32 tileMoldura[8] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};
static const u32 tileVazio[8] = {
    0xF111111F, 0xF111111F, 0xF111111F, 0xF111111F,
    0xF111111F, 0xF111111F, 0xF111111F, 0xF111111F
};
static const u32 tileCheio[8] = {
    0xF999999F, 0xF999999F, 0xF999999F, 0xF999999F,
    0xF999999F, 0xF999999F, 0xF999999F, 0xF999999F
};

void inicializarTilesBarraCombo(void) {
    // Carrega os 3 blocos na área segura de usuário da VRAM (TILE_USER_INDEX)
    VDP_loadTileData(tileMoldura, TILE_USER_INDEX,     1, CPU);
    VDP_loadTileData(tileVazio,   TILE_USER_INDEX + 1, 1, CPU);
    VDP_loadTileData(tileCheio,   TILE_USER_INDEX + 2, 1, CPU);

    // Cor 1 da PAL0 = cinza escuro (vazio) | Cor 9 da PAL0 = roxo/magenta vibrante (cheio)
    PAL_setColor(1, RGB24_TO_VDPCOLOR(0x111122));
    PAL_setColor(9, RGB24_TO_VDPCOLOR(0xCC33FF));
}

void atualizarBarraCombo(Player* p) {
    PAL_setColor(1, RGB24_TO_VDPCOLOR(0x111122));  // Fundo vazio da barra
    PAL_setColor(9, RGB24_TO_VDPCOLOR(0xCC33FF));  // Preenchimento roxo da barra

    // Garante cores vibrantes para 4X, 5X, 6X, 7X e 8X sem afetar a Aranha
    u32 corDestaque = 0xFFEE00;
    if (p->multiplicador == 4) corDestaque = 0x44AAFF; // Azul claro vibrante
    else if (p->multiplicador == 5) corDestaque = 0x00FFEE; // Ciano neon
    else if (p->multiplicador == 6) corDestaque = 0x44FF44; // Verde limão
    else if (p->multiplicador == 7) corDestaque = 0xFF55FF; // Rosa/Magenta
    else if (p->multiplicador >= 8) corDestaque = 0xFF3333; // Vermelho vivo

    if (p->multiplicador >= 4) {
        PAL_setColor(35, RGB24_TO_VDPCOLOR(corDestaque));
        PAL_setColor(36, RGB24_TO_VDPCOLOR(corDestaque));
        PAL_setColor(37, RGB24_TO_VDPCOLOR(corDestaque));
        PAL_setColor(38, RGB24_TO_VDPCOLOR(corDestaque));
    }

    u8 alturaTotal = 12;
    u8 blocosCheios = 0;

    if (p->multiplicador >= 8) {
        blocosCheios = alturaTotal;
    } else {
        blocosCheios = (p->killsNoNivel * alturaTotal) / p->metaProximoNivel;
    }

    // Topo e base da moldura branca na coluna 37
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX), 37, 8);
    VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USER_INDEX), 37, 21);

    // Preenche de baixo para cima (y = 20 até y = 9)
    for (u8 i = 0; i < alturaTotal; i++) {
        u8 linhaY = 20 - i;
        u16 tileIdx = (i < blocosCheios) ? (TILE_USER_INDEX + 2) : (TILE_USER_INDEX + 1);
        VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, tileIdx), 37, linhaY);
    }

    // Atualiza o ícone 2X..8X acima da barra (x = 288, y = 44)
    if (p->multiplicador >= 2) {
        SPR_setAnim(p->spriteMult, p->multiplicador - 2);
        SPR_setVisibility(p->spriteMult, VISIBLE);
    } else {
        SPR_setVisibility(p->spriteMult, HIDDEN);
    }
}

void registrarAbateCombo(Player* p) {
    if (p->multiplicador < 8) {
        p->killsNoNivel++;
        if (p->killsNoNivel >= p->metaProximoNivel) {
            p->multiplicador++;
            p->killsNoNivel = 0;
            // Meta sobe de 2 em 2: 4 (para 2x), 6 (para 3x), 8 (para 4x)...
            p->metaProximoNivel = 4 + (p->multiplicador - 1) * 2;
        }
    }
    atualizarBarraCombo(p);
}

void resetarCombo(Player* p) {
    p->multiplicador = 1;
    p->killsNoNivel = 0;
    p->metaProximoNivel = 4;
    atualizarBarraCombo(p);
}
