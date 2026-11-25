#include <genesis.h>
#include "resources.h"

#define MAX_TIROS 4
#define MAX_TIROS_INIMIGO 4
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

typedef struct {
    Sprite* sprite;
    s16 x;
    s16 y;
    bool ativo;
    u16 timerRespawn;
    u16 timerTiro;
} Inimigo;

// Posiciona o inimigo aleatoriamente na metade superior da tela (abaixo de "Fase 1!")
void spawnInimigo(Inimigo* inimigo) {
    inimigo->x = 16 + (random() % 265); // Entre 16 e 280 (320 - 24 - 16)
    inimigo->y = 28 + (random() % 77);  // Entre 28 e 104 (metade superior)
    inimigo->ativo = TRUE;
    inimigo->timerTiro = 60;            // Atira 1 segundo (60 frames) após aparecer
    SPR_setPosition(inimigo->sprite, inimigo->x, inimigo->y);
    SPR_setVisibility(inimigo->sprite, VISIBLE);
}

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
        random(); // Varia a semente aleatória conforme o tempo até apertar START

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
    PAL_setPalette(PAL3, spr_n64.palette->data, DMA);

    PAL_setPalette(PAL0, spr_tiro_inimigo_vertical.palette->data, DMA);
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x002266));  // Mantém o azul de fundo
    PAL_setColor(15, RGB24_TO_VDPCOLOR(0xFFFFFF)); // Mantém o texto "Fase 1!" branco

    // Centralizado em X: (320 - 48) / 2 = 136
    // Mais próximo da borda inferior: 224 - 48 - 2 = 174
    s16 x = 136;
    s16 y = 174;
    bool andando = FALSE;
    bool olhandoDireita = TRUE;
    bool botaoTiroTravado = FALSE;
    u16 timerPiscaDano = 0;

    Sprite* player = SPR_addSprite(&spr_idle, x, y, TILE_ATTR(PAL1, FALSE, FALSE, TRUE));

    // Pré-aloca os sprites dos tiros do jogador
    Tiro tiros[MAX_TIROS];
    for (u8 i = 0; i < MAX_TIROS; i++) {
        tiros[i].ativo = FALSE;
        tiros[i].sprite = SPR_addSprite(&spr_tiro, 0, -16, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
        SPR_setVisibility(tiros[i].sprite, HIDDEN);
    }

    // Pré-aloca os sprites dos tiros do inimigo
    Tiro tirosInimigo[MAX_TIROS_INIMIGO];
    for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
        tirosInimigo[i].ativo = FALSE;
        tirosInimigo[i].sprite = SPR_addSprite(&spr_tiro_inimigo_vertical, 0, -16, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
        SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
    }

    // Inicializa o inimigo N64 (24x24) e faz o primeiro aparecer
    Inimigo n64;
    n64.timerRespawn = 0;
    n64.sprite = SPR_addSprite(&spr_n64, 0, -24, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
    spawnInimigo(&n64);

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

                        // Alinha o tiro (16x16) com o canhão no sprite de 48x48
                        if (olhandoDireita) {
                            tiros[i].x = x + 14;
                        } else {
                            tiros[i].x = x + 18;
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

        // Atualiza tiros do jogador e verifica colisão com o N64
        for (u8 i = 0; i < MAX_TIROS; i++) {
            if (tiros[i].ativo) {
                tiros[i].y += tiros[i].vy;

                if (tiros[i].y < -16) {
                    SPR_setVisibility(tiros[i].sprite, HIDDEN);
                    tiros[i].ativo = FALSE;
                } else {
                    SPR_setPosition(tiros[i].sprite, tiros[i].x, tiros[i].y);

                    // Colisão entre a área útil do tiro (12x12) e o N64 (24x24)
                    if (n64.ativo &&
                        tiros[i].x + 14 > n64.x && tiros[i].x + 2 < n64.x + 24 &&
                        tiros[i].y + 14 > n64.y && tiros[i].y + 2 < n64.y + 24)
                    {
                        tiros[i].ativo = FALSE;
                        SPR_setVisibility(tiros[i].sprite, HIDDEN);

                        n64.ativo = FALSE;
                        n64.timerRespawn = 20; // Aguarda ~0,3s para surgir o próximo
                        SPR_setVisibility(n64.sprite, HIDDEN);
                    }
                }
            }
        }

        // Lógica do inimigo N64 (disparo a cada 1 segundo e respawn)
        if (n64.ativo) {
            if (n64.timerTiro > 0) {
                n64.timerTiro--;
            } else {
                n64.timerTiro = 60; // Reinicia o timer de 1 segundo (60 frames)

                for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
                    if (!tirosInimigo[i].ativo) {
                        tirosInimigo[i].ativo = TRUE;
                        // Alinha o centro do tiro (16x16) com o pino central inferior do N64 (24x24)
                        tirosInimigo[i].x = n64.x + 4;
                        tirosInimigo[i].y = n64.y + 20;
                        tirosInimigo[i].vy = 3; // Velocidade de descida do tiro inimigo

                        SPR_setPosition(tirosInimigo[i].sprite, tirosInimigo[i].x, tirosInimigo[i].y);
                        SPR_setVisibility(tirosInimigo[i].sprite, VISIBLE);
                        break;
                    }
                }
            }
        } else {
            if (n64.timerRespawn > 0) {
                n64.timerRespawn--;
            } else {
                spawnInimigo(&n64);
            }
        }

        // Atualiza tiros do inimigo e verifica colisão com o jogador
        for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
            if (tirosInimigo[i].ativo) {
                tirosInimigo[i].y += tirosInimigo[i].vy;

                if (tirosInimigo[i].y > 224) {
                    SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
                    tirosInimigo[i].ativo = FALSE;
                } else {
                    SPR_setPosition(tirosInimigo[i].sprite, tirosInimigo[i].x, tirosInimigo[i].y);

                    // Colisão com a hitbox central (16x16) do jogador (48x48)
                    if (tirosInimigo[i].x + 12 > x + 16 && tirosInimigo[i].x + 4 < x + 32 &&
                        tirosInimigo[i].y + 12 > y + 16 && tirosInimigo[i].y + 4 < y + 32)
                    {
                        tirosInimigo[i].ativo = FALSE;
                        SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
                        timerPiscaDano = 30; // Pisca o jogador por 0,5s ao tomar dano
                    }
                }
            }
        }

        // Efeito visual de piscar quando o jogador leva dano
        if (timerPiscaDano > 0) {
            timerPiscaDano--;
            SPR_setVisibility(player, (timerPiscaDano % 4 < 2) ? HIDDEN : VISIBLE);
        } else {
            SPR_setVisibility(player, VISIBLE);
        }

        // Limites laterais para sprite de 48px na tela de 320px (320 - 48 = 272)
        if (x < 0)   x = 0;
        if (x > 272) x = 272;

        SPR_setPosition(player, x, y);
        SPR_update();

        SYS_doVBlankProcess();
    }

    return 0;
}
