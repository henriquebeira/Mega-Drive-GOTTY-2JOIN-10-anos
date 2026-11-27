#include "jogo.h"

#define YM_ADDR_1 ((volatile u8*)0xA04000)
#define YM_DATA_1 ((volatile u8*)0xA04001)
#define PSG_PORT  ((volatile u8*)0xC00011)

static u8 stepJazz = 0;
static u8 timerPasso = 0;
static u8 timerHihat = 0;

static void ym_write(u8 reg, u8 val) {
    while (*YM_ADDR_1 & 0x80);
    *YM_ADDR_1 = reg;
    while (*YM_ADDR_1 & 0x80);
    *YM_DATA_1 = val;
}

static void fm_note_on(u8 ch, u8 msb, u8 lsb) {
    ym_write(0x28, ch);
    ym_write(0xA4 + ch, msb);
    ym_write(0xA0 + ch, lsb);
    ym_write(0x28, 0xF0 | ch);
}

static void fm_note_off(u8 ch) {
    ym_write(0x28, ch);
}

static const u8 D2[2] = {0x12, 0xDD};
static const u8 F2[2] = {0x13, 0x57};
static const u8 G2[2] = {0x13, 0xC9};
static const u8 A2[2] = {0x14, 0x46};
static const u8 C3[2] = {0x15, 0x22};
static const u8 F4[2] = {0x23, 0x57};
static const u8 E5[2] = {0x2A, 0x8E};
static const u8 B4[2] = {0x24, 0xD2};

void iniciarMusicaSecretaJazz(void) {
    // Para a música PCM normal e assume o barramento do Z80 para escrever direto no YM2612
    XGM_stopPlayPCM(SOUND_PCM_CH2);
    Z80_requestBus(TRUE);

    // Canal 0: Baixo Slap FM
    ym_write(0xB0, 0x02);
    ym_write(0xB4, 0xC0);
    ym_write(0x30, 0x02); ym_write(0x40, 0x18); ym_write(0x50, 0x1F); ym_write(0x60, 0x0C); ym_write(0x80, 0x0F);
    ym_write(0x38, 0x01); ym_write(0x48, 0x00); ym_write(0x58, 0x1F); ym_write(0x68, 0x07); ym_write(0x88, 0x0F);

    // Canais 1 e 2: Piano Elétrico FM
    for (u8 ch = 1; ch <= 2; ch++) {
        ym_write(0xB0 + ch, 0x04);
        ym_write(0xB4 + ch, 0xC0);
        ym_write(0x30 + ch, 0x01); ym_write(0x40 + ch, 0x24); ym_write(0x50 + ch, 0x1A); ym_write(0x60 + ch, 0x05); ym_write(0x80 + ch, 0x08);
        ym_write(0x38 + ch, 0x01); ym_write(0x48 + ch, 0x08); ym_write(0x58 + ch, 0x1C); ym_write(0x68 + ch, 0x04); ym_write(0x88 + ch, 0x08);
        ym_write(0x34 + ch, 0x07); ym_write(0x44 + ch, 0x2A); ym_write(0x54 + ch, 0x1F); ym_write(0x64 + ch, 0x0D); ym_write(0x84 + ch, 0x0A);
        ym_write(0x3C + ch, 0x01); ym_write(0x4C + ch, 0x0A); ym_write(0x5C + ch, 0x1C); ym_write(0x6C + ch, 0x05); ym_write(0x8C + ch, 0x08);
    }

    // PSG Canal 3: Chimbal
    *PSG_PORT = 0xE4;
    *PSG_PORT = 0xFF;

    stepJazz = 0;
    timerPasso = 0;
    timerHihat = 0;
}

void atualizarMusicaSecretaJazz(void) {
    // Controla o desligamento suave do chimbal sem travar o frame
    if (timerHihat > 0) {
        timerHihat--;
        if (timerHihat == 0) {
            *PSG_PORT = 0xFF;
        }
    }

    if (timerPasso > 0) {
        timerPasso--;
        return;
    }

    // 1. Baixo Sincopado (Canal 0)
    switch (stepJazz) {
        case 0:  fm_note_on(0, D2[0], D2[1]); break;
        case 3:  fm_note_on(0, C3[0], C3[1]); break;
        case 4:  fm_note_on(0, D2[0], D2[1]); break;
        case 6:  fm_note_on(0, F2[0], F2[1]); break;
        case 7:  fm_note_on(0, A2[0], A2[1]); break;
        case 8:  fm_note_on(0, G2[0], G2[1]); break;
        case 11: fm_note_on(0, F2[0], F2[1]); break;
        case 12: fm_note_on(0, G2[0], G2[1]); break;
        case 14: fm_note_on(0, A2[0], A2[1]); break;
        default: fm_note_off(0); break;
    }

    // 2. Piano Elétrico (Canais 1 e 2)
    if (stepJazz == 0 || stepJazz == 6) {
        fm_note_on(1, F4[0], F4[1]);
        fm_note_on(2, E5[0], E5[1]);
    } else if (stepJazz == 8 || stepJazz == 13) {
        fm_note_on(1, B4[0], B4[1]);
        fm_note_on(2, E5[0], E5[1]);
    } else if (stepJazz == 4 || stepJazz == 11) {
        fm_note_off(1);
        fm_note_off(2);
    }

    // 3. Chimbal + Groove (8 frames ~= 130ms)
    if (stepJazz % 2 == 0) {
        *PSG_PORT = 0xF2;
        timerHihat = (stepJazz == 14) ? 3 : 1;
        timerPasso = 8;
    } else {
        timerPasso = 8;
    }

    stepJazz = (stepJazz + 1) % 16;
}

void pararMusicaSecretaJazz(void) {
    fm_note_off(0);
    fm_note_off(1);
    fm_note_off(2);
    *PSG_PORT = 0xFF;
    // Devolve o barramento para o Z80/XGM antes de iniciar o jogo!
    Z80_releaseBus();
}
