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

    const char* tituloFim = "100 2JOIN - FIM DE JOGO!";
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
    PAL_setColor(1, RGB24_TO_VDPCOLOR(0x111122)); // Fundo vazio da barra
    PAL_setColor(9, RGB24_TO_VDPCOLOR(0xCC33FF)); // Preenchimento roxo da barra

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
            // Toca o "Toasty!" no canal 3 (prioridade 15) sem interromper música ou tiros
            XGM_startPlayPCM(SFX_TOASTY_ID, 15, SOUND_PCM_CH3);
            p->timerToasty = 65; // Faz o Visio_Toasty subir no canto direito!
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

// Interrupção horizontal na linha 164: troca a PAL3 para o Toasty apenas na parte inferior da tela
HINTERRUPT_CALLBACK onHBlankToasty(void) {
    PAL_setPalette(PAL3, spr_toasty.palette->data, CPU);
}

void atualizarAnimacaoToasty(Player* p) {
    if (p->timerToasty > 0) {
        p->timerToasty--;

        s16 posY = 224;
        if (p->timerToasty > 50) {
            posY = 160 + ((p->timerToasty - 50) * 4);
        } else if (p->timerToasty > 15) {
            posY = 160;
        } else {
            posY = 160 + ((15 - p->timerToasty) * 4);
        }

        SPR_setPosition(p->spriteToasty, 248, posY);
        SPR_setVisibility(p->spriteToasty, VISIBLE);
    } else {
        SPR_setVisibility(p->spriteToasty, HIDDEN);
    }
}
static u8 timerSomDanoPSG = 0;

void tocarSomDanoPSG(void) {
    timerSomDanoPSG = 9; // Impacto rápido e com volume máximo

    // Soma os 3 canais de onda quadrada (0, 1 e 2) + ruído (3) no volume máximo (0)
    PSG_setFrequency(0, 380);
    PSG_setFrequency(1, 385);
    PSG_setFrequency(2, 190); // Uma oitava abaixo para dar peso

    PSG_setEnvelope(0, PSG_ENVELOPE_MAX);
    PSG_setEnvelope(1, PSG_ENVELOPE_MAX);
    PSG_setEnvelope(2, PSG_ENVELOPE_MAX);

    PSG_setNoise(PSG_NOISE_TYPE_WHITE, PSG_NOISE_FREQ_CLOCK4);
    PSG_setEnvelope(3, PSG_ENVELOPE_MAX);
}

void atualizarSomDanoPSG(void) {
    if (timerSomDanoPSG > 0) {
        timerSomDanoPSG--;

        u16 freq = 50;
        if (timerSomDanoPSG == 8)      freq = 280;
        else if (timerSomDanoPSG == 7) freq = 200;
        else if (timerSomDanoPSG == 6) freq = 140;
        else if (timerSomDanoPSG == 5) freq = 100;
        else if (timerSomDanoPSG == 4) freq = 75;
        else if (timerSomDanoPSG == 3) freq = 55;

        PSG_setFrequency(0, freq);
        PSG_setFrequency(1, freq + 4);
        PSG_setFrequency(2, freq / 2);

        // Mantém o volume máximo nos primeiros frames e atenua no final
        u8 env = (timerSomDanoPSG > 4) ? 0 : ((4 - timerSomDanoPSG) * 3);
        if (env > 15) env = 15;

        PSG_setEnvelope(0, env);
        PSG_setEnvelope(1, env);
        PSG_setEnvelope(2, env);

        if (timerSomDanoPSG <= 4) {
            PSG_setEnvelope(3, PSG_ENVELOPE_MIN);
        } else {
            PSG_setEnvelope(3, env);
        }

        if (timerSomDanoPSG == 0) {
            PSG_setEnvelope(0, PSG_ENVELOPE_MIN);
            PSG_setEnvelope(1, PSG_ENVELOPE_MIN);
            PSG_setEnvelope(2, PSG_ENVELOPE_MIN);
            PSG_setEnvelope(3, PSG_ENVELOPE_MIN);
        }
    }
}

static u8 timerEstilhacos = 0;

void dispararEstilhacos(Estilhaco estilhacos[], const SpriteDefinition* spriteDef, u8 paleta, s16 centroX, s16 centroY) {
    static const s8 velX[4] = { -3,  3, -2,  2 };
    static const s8 velY[4] = { -3, -3,  1,  1 };
    static const bool hFlip[4] = { FALSE, TRUE, FALSE, TRUE };
    static const bool vFlip[4] = { FALSE, FALSE, TRUE, TRUE };

    timerEstilhacos = 14; // Duração do estilhaço (~0,23s)

    for (u8 i = 0; i < MAX_ESTILHACOS; i++) {
        SPR_setDefinition(estilhacos[i].sprite, spriteDef);
        SPR_setPalette(estilhacos[i].sprite, paleta);
        SPR_setHFlip(estilhacos[i].sprite, hFlip[i]);
        SPR_setVFlip(estilhacos[i].sprite, vFlip[i]);

        estilhacos[i].x = centroX - 12;
        estilhacos[i].y = centroY - 12;
        estilhacos[i].vx = velX[i];
        estilhacos[i].vy = velY[i];

        SPR_setPosition(estilhacos[i].sprite, estilhacos[i].x, estilhacos[i].y);
        SPR_setVisibility(estilhacos[i].sprite, VISIBLE);
    }
}

void inicializarEstilhacos(Estilhaco estilhacos[]) {
    for (u8 i = 0; i < MAX_ESTILHACOS; i++) {
        estilhacos[i].sprite = SPR_addSprite(&spr_tiros, 0, -16, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
        SPR_setAnim(estilhacos[i].sprite, 2);
        SPR_setVisibility(estilhacos[i].sprite, HIDDEN);
    }
}

void atualizarEstilhacos(Estilhaco estilhacos[]) {
    if (timerEstilhacos > 0) {
        timerEstilhacos--;

        for (u8 i = 0; i < MAX_ESTILHACOS; i++) {
            estilhacos[i].x += estilhacos[i].vx;
            estilhacos[i].y += estilhacos[i].vy;
            estilhacos[i].vy++;

            SPR_setPosition(estilhacos[i].sprite, estilhacos[i].x, estilhacos[i].y);
            if (timerEstilhacos < 6) {
                SPR_setVisibility(estilhacos[i].sprite, (timerEstilhacos % 2 == 0) ? VISIBLE : HIDDEN);
            }
        }

        if (timerEstilhacos == 0) {
            for (u8 i = 0; i < MAX_ESTILHACOS; i++) {
                SPR_setVisibility(estilhacos[i].sprite, HIDDEN);
            }
        }
    }
}

