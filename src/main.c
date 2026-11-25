#include <genesis.h>
#include "resources.h"

#define MAX_TIROS 4
#define SFX_TIRO_ID 64
#define BGM_MENU_ID 65
#define BGM_JOGO_ID 66

typedef struct {
    Sprite* sprite;
    s16 x;
    s16 y;
    s16 vy;
    bool ativo;
} Tiro;

int main(bool hard)
{
    JOY_init();
    SPR_init();

    // Registra o som do tiro e as duas músicas nos IDs PCM do XGM
    XGM_setPCM(SFX_TIRO_ID, sfx_tiro, sizeof(sfx_tiro));
    XGM_setPCM(BGM_MENU_ID, bgm_menu, sizeof(bgm_menu));
    XGM_setPCM(BGM_JOGO_ID, bgm_jogo, sizeof(bgm_jogo));

    // --- TELA INICIAL (MENU) ---
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x001133));
    VDP_drawText("MEU JOGO MEGA DRIVE", 10, 10);
    VDP_drawText("PRESS START", 14, 14);

    // Inicia a música do menu no Canal 2
    XGM_startPlayPCM(BGM_MENU_ID, 1, SOUND_PCM_CH2);

    // Aguarda apertar START e mantém a música em loop
    while(TRUE)
    {
        if (!XGM_isPlayingPCM(SOUND_PCM_CH2_MSK)) {
            XGM_startPlayPCM(BGM_MENU_ID, 1, SOUND_PCM_CH2);
        }

        u16 controle = JOY_readJoypad(JOY_1);
        if (controle & BUTTON_START) break;
        SYS_doVBlankProcess();
    }

    // Troca a música do menu pela música da fase e limpa a tela
    XGM_stopPlayPCM(SOUND_PCM_CH2);
    VDP_clearTextArea(0, 0, 40, 28);
    XGM_startPlayPCM(BGM_JOGO_ID, 1, SOUND_PCM_CH2);

    // --- INÍCIO DA FASE ---
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x002266));
    VDP_drawText("Fase 1!", 16, 2);

    PAL_setPalette(PAL1, spr_idle.palette->data, DMA);
    PAL_setPalette(PAL2, spr_tiro.palette->data, DMA);

    // Centralizado em X (320 - 64) / 2 = 128 e inteiro acima da borda inferior (224 - 64 - 4 = 156)
    s16 x = 128;
    s16 y = 156;
    bool andando = FALSE;
    bool olhandoDireita = TRUE;
    bool botaoTiroTravado = FALSE;

    Sprite* player = SPR_addSprite(&spr_idle, x, y, TILE_ATTR(PAL1, FALSE, FALSE, TRUE));

    // Pré-aloca os sprites dos tiros para evitar fragmentação de memória
    Tiro tiros[MAX_TIROS];
    for (u8 i = 0; i < MAX_TIROS; i++) {
        tiros[i].ativo = FALSE;
        tiros[i].sprite = SPR_addSprite(&spr_tiro, 0, -16, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
        SPR_setVisibility(tiros[i].sprite, HIDDEN);
    }

    while(TRUE)
    {
        // Garante o loop contínuo da música da fase
        if (!XGM_isPlayingPCM(SOUND_PCM_CH2_MSK)) {
            XGM_startPlayPCM(BGM_JOGO_ID, 1, SOUND_PCM_CH2);
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
            SPR_setDefinition(player, &spr_run);
            andando = TRUE;
        } else if (!movendoAgora && andando) {
            SPR_setDefinition(player, &spr_idle);
            andando = FALSE;
        }

        if (controle & (BUTTON_B | BUTTON_A)) {
            if (!botaoTiroTravado) {
                botaoTiroTravado = TRUE;

                for (u8 i = 0; i < MAX_TIROS; i++) {
                    if (!tiros[i].ativo) {
                        tiros[i].ativo = TRUE;
                        tiros[i].y = y - 8;
                        tiros[i].vy = -5;

                        // Alinha o tiro de 16x16 com o canhão no sprite de 64px
                        if (olhandoDireita) {
                            tiros[i].x = x + 1;
                        } else {
                            tiros[i].x = x + 47;
                        }

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

        // Limites laterais para sprite de 64px na tela de 320px (320 - 64 = 256)
        if (x < 0)   x = 0;
        if (x > 256) x = 256;

        SPR_setPosition(player, x, y);
        SPR_update();

        SYS_doVBlankProcess();
    }

    return 0;
}
