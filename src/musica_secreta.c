#include "jogo.h"

#define YM_ADDR_1 ((volatile u8*)0xA04000)
#define YM_DATA_1 ((volatile u8*)0xA04001)
#define PSG_PORT  ((volatile u8*)0xC00011)

// Notas Synth 80s (Botão Y)
#define NOTE_F2  0x1269
#define NOTE_GS2 0x12E1
#define NOTE_AS2 0x133C
#define NOTE_C3  0x13A2
#define NOTE_DS3 0x2226
#define NOTE_F3  0x2269
#define NOTE_GS3 0x22E1
#define NOTE_AS3 0x233C
#define NOTE_C4  0x23A2
#define REST     0x0000

static u8 stepSecreta = 0;
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

static void play_note_16(u8 channel, u16 freq) {
    ym_write(0x28, channel);
    if (freq == REST) return;
    ym_write(0xA4 + channel, (freq >> 8) & 0xFF);
    ym_write(0xA0 + channel, freq & 0xFF);
    ym_write(0x28, 0xF0 | channel);
}

// --- MÚSICA 1: JAZZ FUSION (BOTÃO X) ---
void iniciarMusicaSecretaJazz(void) {
    XGM_stopPlayPCM(SOUND_PCM_CH2);
    Z80_requestBus(TRUE);

    fm_note_off(0);
    fm_note_off(1);
    fm_note_off(2);
    *PSG_PORT = 0xFF;

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

    *PSG_PORT = 0xE4;
    *PSG_PORT = 0xFF;

    stepSecreta = 0;
    timerPasso = 0;
    timerHihat = 0;
}

void atualizarMusicaSecretaJazz(void) {
    if (timerHihat > 0) {
        timerHihat--;
        if (timerHihat == 0) *PSG_PORT = 0xFF;
    }

    if (timerPasso > 0) {
        timerPasso--;
        return;
    }

    switch (stepSecreta) {
        case 0:  fm_note_on(0, 0x12, 0xDD); break; // D2
        case 3:  fm_note_on(0, 0x15, 0x22); break; // C3
        case 4:  fm_note_on(0, 0x12, 0xDD); break; // D2
        case 6:  fm_note_on(0, 0x13, 0x57); break; // F2
        case 7:  fm_note_on(0, 0x14, 0x46); break; // A2
        case 8:  fm_note_on(0, 0x13, 0xC9); break; // G2
        case 11: fm_note_on(0, 0x13, 0x57); break; // F2
        case 12: fm_note_on(0, 0x13, 0xC9); break; // G2
        case 14: fm_note_on(0, 0x14, 0x46); break; // A2
        default: fm_note_off(0); break;
    }

    if (stepSecreta == 0 || stepSecreta == 6) {
        fm_note_on(1, 0x23, 0x57); // F4
        fm_note_on(2, 0x2A, 0x8E); // E5
    } else if (stepSecreta == 8 || stepSecreta == 13) {
        fm_note_on(1, 0x24, 0xD2); // B4
        fm_note_on(2, 0x2A, 0x8E); // E5
    } else if (stepSecreta == 4 || stepSecreta == 11) {
        fm_note_off(1);
        fm_note_off(2);
    }

    if (stepSecreta % 2 == 0) {
        *PSG_PORT = 0xF2;
        timerHihat = (stepSecreta == 14) ? 3 : 1;
        timerPasso = 8;
    } else {
        timerPasso = 8;
    }

    stepSecreta = (stepSecreta + 1) % 16;
}

// --- MÚSICA 2: SYNTH BASS & BRASS ANOS 80 (BOTÃO Y) ---
static u16 getBass80s(u8 step) {
    switch (step) {
        case 0: case 2: return NOTE_F2;
        case 3: case 6: case 14: return NOTE_DS3;
        case 4: return NOTE_F3;
        case 7: case 12: return NOTE_C3;
        case 8: case 10: return NOTE_AS2;
        case 11: return NOTE_GS2;
        default: return REST;
    }
}

static u16 getLead80s(u8 step) {
    switch (step) {
        case 2: case 8: case 11: return NOTE_F3;
        case 3: case 7: case 12: return NOTE_GS3;
        case 4: case 6: return NOTE_AS3;
        case 5: return NOTE_C4;
        case 10: return NOTE_DS3;
        default: return REST;
    }
}

void iniciarMusicaSecretaSynth80(void) {
    XGM_stopPlayPCM(SOUND_PCM_CH2);
    Z80_requestBus(TRUE);

    fm_note_off(0);
    fm_note_off(1);
    fm_note_off(2);
    *PSG_PORT = 0xFF;

    // Canal 0: Synth Bass anos 80
    ym_write(0xB0, 0x04);
    ym_write(0xB4, 0xC0);
    ym_write(0x30, 0x71); ym_write(0x40, 0x1C); ym_write(0x50, 0x1F);
    ym_write(0x60, 0x0A); ym_write(0x70, 0x05); ym_write(0x80, 0x2F);
    ym_write(0x38, 0x31); ym_write(0x48, 0x00); ym_write(0x58, 0x1F);
    ym_write(0x68, 0x08); ym_write(0x78, 0x04); ym_write(0x88, 0x0F);

    // Canal 1: Synth Brass / Lead
    ym_write(0xB1, 0x02);
    ym_write(0xB5, 0xC0);
    ym_write(0x31, 0x41); ym_write(0x41, 0x18); ym_write(0x51, 0x18);
    ym_write(0x61, 0x06); ym_write(0x81, 0x0F);
    ym_write(0x39, 0x01); ym_write(0x49, 0x04); ym_write(0x59, 0x1A);
    ym_write(0x69, 0x05); ym_write(0x89, 0x0F);

    stepSecreta = 0;
    timerPasso = 0;
}

void atualizarMusicaSecretaSynth80(void) {
    if (timerPasso == 0) {
        play_note_16(0, getBass80s(stepSecreta));
        play_note_16(1, getLead80s(stepSecreta));
        stepSecreta = (stepSecreta + 1) % 16;
    }

    timerPasso++;
    if (timerPasso >= 8) {
        timerPasso = 0;
    }
}

void pararMusicaSecreta(void) {
    // 1. Garante que o 68000 tem acesso ao YM2612
    Z80_requestBus(TRUE);

    // 2. Silencia imediatamente todos os 6 canais FM (Key Off + Volume máximo de atenuação 0x7F)
    for (u8 ch = 0; ch < 3; ch++) {
        ym_write(0x28, ch);       // Key Off canais 0..2
        ym_write(0x28, 0x04 | ch); // Key Off canais 3..5

        // Zera volume (Total Level = 0x7F) nos 4 operadores da Parte 1
        ym_write(0x40 + ch, 0x7F);
        ym_write(0x44 + ch, 0x7F);
        ym_write(0x48 + ch, 0x7F);
        ym_write(0x4C + ch, 0x7F);
    }

    // 3. Silencia os 4 canais do chip PSG (SN76489)
    *PSG_PORT = 0x9F;
    *PSG_PORT = 0xBF;
    *PSG_PORT = 0xDF;
    *PSG_PORT = 0xFF;

    // 4. Devolve o barramento para o Z80 / XGM
    Z80_releaseBus();
}

// --- MÚSICA 3: PLAYER PSGMOD NATIVO (BOTÃO Z) ---
static const u8* psgData = NULL;
static u32 psgSize = 0;
static u32 psgPos = 0;
static u32 psgLoopPos = 0;
static u16 psgDelay = 0;

void iniciarMusicaSecretaPSG(void) {
    XGM_stopPlayPCM(SOUND_PCM_CH2);
    // Para qualquer música FM anterior se estava tocando X ou Y
    pararMusicaSecreta();

    // Silencia os 4 canais do SN76489 (PSG)
    *PSG_PORT = 0x9F;
    *PSG_PORT = 0xBF;
    *PSG_PORT = 0xDF;
    *PSG_PORT = 0xFF;

    psgData = psgmod_z;
    psgSize = sizeof(psgmod_z);
    psgPos = 0;
    psgDelay = 0;

    // Localiza a assinatura "PATT" no cabeçalho PSGMOD-2HEAD onde começam os dados das notas
    for (u32 i = 0; i + 4 < psgSize; i++) {
        if (psgData[i] == 'P' && psgData[i + 1] == 'A' &&
            psgData[i + 2] == 'T' && psgData[i + 3] == 'T') {
            psgPos = i + 4;
            break;
        }
    }
    psgLoopPos = psgPos;
}

void atualizarMusicaSecretaPSG(void) {
    if (psgData == NULL || psgPos == 0) return;

    if (psgDelay > 0) {
        psgDelay--;
        return;
    }

    // Lê os comandos do frame atual até encontrar o marcador de fim de frame/espera
    while (psgPos < psgSize) {
        u8 cmd = psgData[psgPos++];

        if (cmd == 0xFF) {
            // Fim da música -> volta para o início do padrão
            psgPos = psgLoopPos;
            break;
        } else if (cmd == 0xFE) {
            // Espera N frames
            if (psgPos < psgSize) {
                psgDelay = psgData[psgPos++];
            }
            break;
        } else if (cmd & 0x80) {
            // Comando de latch do SN76489 (escreve direto na porta 0xC00011)
            *PSG_PORT = cmd;
            // Se for comando de tom (bit 4 == 0 e canal < 3), envia também o 2º byte de frequência
            if ((cmd & 0x10) == 0 && (cmd & 0x60) != 0x60 && psgPos < psgSize) {
                *PSG_PORT = psgData[psgPos++];
            }
        } else {
            // Atraso curto entre notas
            psgDelay = cmd;
            break;
        }
    }
}

void pararMusicaSecretaPSG(void) {
    *PSG_PORT = 0x9F;
    *PSG_PORT = 0xBF;
    *PSG_PORT = 0xDF;
    *PSG_PORT = 0xFF;
}
