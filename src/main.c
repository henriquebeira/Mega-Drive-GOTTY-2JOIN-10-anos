#include "jogo.h"

int main(bool hard)
{
    JOY_init();
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

        // --- INÍCIO DA FASE ---
        XGM_startPlayPCM(BGM_JOGO_ID, 1, SOUND_PCM_CH2);

        u16 pontos = 0;
        u16 edicao = 1;
        u16 timerEdicao = 0;

        PAL_setPalette(PAL1, spr_idle.palette->data, DMA);
        PAL_setPalette(PAL2, spr_aranha.palette->data, DMA);
        PAL_setPalette(PAL3, spr_disjuntor.palette->data, DMA);

        SYS_setHIntCallback(onHBlankToasty);
        VDP_setHIntCounter(164);
        VDP_setHInterrupt(TRUE);

        PAL_setPalette(PAL0, spr_tiros.palette->data, DMA);
        PAL_setColor(0, RGB24_TO_VDPCOLOR(0x002266));
        PAL_setColor(15, RGB24_TO_VDPCOLOR(0xFFFFFF));

        atualizarContadorEdicao(edicao);
        atualizarPlacar(pontos);    

        Player player;
        Tiro tiros[MAX_TIROS];
     inicializarTilesBarraCombo();
     inicializarPlayer(&player, tiros);
     atualizarBarraCombo(&player);        

        Tiro tirosInimigo[MAX_TIROS_INIMIGO];
        for (u8 i = 0; i < MAX_TIROS_INIMIGO; i++) {
            tirosInimigo[i].ativo = FALSE;
            tirosInimigo[i].vx = 0;
            tirosInimigo[i].sprite = SPR_addSprite(&spr_tiros, 0, -16, TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
            SPR_setVisibility(tirosInimigo[i].sprite, HIDDEN);
        }

        Inimigo n64;
        n64.timerRespawn = 0;
        n64.timerPisca = 0;
        n64.sprite = SPR_addSprite(&spr_n64, 0, -32, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
        spawnN64(&n64);

        Inimigo aranha;
        aranha.ativo = FALSE;
        aranha.timerRespawn = 0;
        aranha.timerPisca = 0;
        aranha.sprite = SPR_addSprite(&spr_aranha, 0, -32, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
        SPR_setVisibility(aranha.sprite, HIDDEN);

        Inimigo disjuntor;
        disjuntor.ativo = FALSE;
        disjuntor.timerRespawn = 0;
        disjuntor.timerPisca = 0;
        disjuntor.sprite = SPR_addSprite(&spr_disjuntor, 0, -32, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
        SPR_setVisibility(disjuntor.sprite, HIDDEN);

        Inimigo pneu;
        pneu.ativo = FALSE;
        pneu.timerRespawn = 0;
        pneu.timerPisca = 0;
        pneu.sprite = SPR_addSprite(&spr_pneu, 0, -32, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
        SPR_setVisibility(pneu.sprite, HIDDEN);

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

                if (edicao >= 100) {
                    break;
                }
            }

            atualizarPlayerETiros(&player, tiros, &n64, &aranha, &disjuntor, &pneu, estilhacos, &pontos);
            atualizarInimigosETiros(&n64, &aranha, &disjuntor, &pneu, tirosInimigo, &player, edicao);

             atualizarSomDanoPSG();
             PAL_setPalette(PAL3, spr_disjuntor.palette->data, DMA);
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
