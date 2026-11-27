#include "jogo.h"

int main(bool hard)
{
    JOY_init();
    JOY_setSupport(PORT_1, JOY_SUPPORT_6BTN);
    SPR_init();

    XGM_setPCM(SFX_TIRO_ID, sfx_tiro, sizeof(sfx_tiro));
    XGM_setPCM(BGM_MENU_ID, bgm_menu, sizeof(bgm_menu));
    XGM_setPCM(BGM_JOGO_ID, bgm_jogo, sizeof(bgm_jogo));
    XGM_setPCM(SFX_TOASTY_ID, sfx_toasty, sizeof(sfx_toasty));
    XGM_setPCM(SFX_DANO_ID, sfx_dano, sizeof(sfx_dano));
    XGM_setPCM(SFX_EXPLOSAO_ID, sfx_explosao, sizeof(sfx_explosao));

    while(TRUE)
    {
        executarTelaInicial();
        SPR_reset();
        inicializarFonteHUD();

        VDP_clearPlane(BG_A, TRUE);
        VDP_clearPlane(BG_B, TRUE);

        Inimigo n64[MAX_N64];
        Inimigo aranha[MAX_ARANHA];
        Inimigo disjuntor[MAX_DISJUNTOR];
        Inimigo pneu;
        Tiro tirosInimigo[MAX_TIROS_INIMIGO];

        inicializarInimigos(n64, aranha, disjuntor, &pneu, tirosInimigo);

        // --- INÍCIO DA FASE ---
        u32 pontos = 0;
        u16 edicao = 1;
        u16 timerEdicao = 0;

        // Paletas originais dedicadas:
        // PAL0 = Tiros e HUD | PAL1 = Gotty e Pneu | PAL2 = Aranha e N64 | PAL3 = Disjuntor
        PAL_setPalette(PAL0, spr_tiros.palette->data, CPU);
        PAL_setPalette(PAL1, spr_idle.palette->data, CPU);
        PAL_setPalette(PAL2, spr_aranha.palette->data, CPU);
        PAL_setPalette(PAL3, spr_disjuntor.palette->data, CPU);

        VDP_setBackgroundColor(0);
        PAL_setColor(0, RGB24_TO_VDPCOLOR(0x002266));
        PAL_setColor(15, RGB24_TO_VDPCOLOR(0xFFFFFF));

        SYS_setHIntCallback(onHBlankToasty);
        VDP_setHIntCounter(164);
        VDP_setHInterrupt(TRUE);

        atualizarContadorEdicao(edicao);
        atualizarPlacar(pontos);

        Player player;
        Tiro tiros[MAX_TIROS];

        inicializarTilesBarraCombo();
        inicializarPlayer(&player, tiros);

        Estilhaco estilhacos[MAX_ESTILHACOS];
        inicializarEstilhacos(estilhacos);
        atualizarBarraCombo(&player);

        XGM_startPlayPCM(BGM_JOGO_ID, 1, SOUND_PCM_CH2);

        while(TRUE)
        {
            if (edicao > 2 && !XGM_isPlayingPCM(SOUND_PCM_CH2_MSK)) {
                break;
            }

            timerEdicao++;
            if (timerEdicao >= 60) {
                timerEdicao = 0;
                edicao++;
                atualizarContadorEdicao(edicao);

                if (edicao >= 100) {
                    break;
                }
            }

            atualizarPlayerETiros(&player, tiros, n64, aranha, disjuntor, &pneu, estilhacos, &pontos);
            atualizarInimigosETiros(n64, aranha, disjuntor, &pneu, tirosInimigo, &player, edicao);
            atualizarEstilhacos(estilhacos);

            atualizarSomDanoPSG();
            SPR_update();
            SYS_doVBlankProcess();
        }

        VDP_setHInterrupt(FALSE);
        PSG_setEnvelope(3, PSG_ENVELOPE_MIN);
        PSG_setEnvelope(2, PSG_ENVELOPE_MIN);
        PSG_setEnvelope(1, PSG_ENVELOPE_MIN);
        PSG_setEnvelope(0, PSG_ENVELOPE_MIN);

        while (XGM_isPlayingPCM(SOUND_PCM_CH2_MSK)) {
            SYS_doVBlankProcess();
        }

        executarTelaFinal(pontos);
    }

    return 0;
}
