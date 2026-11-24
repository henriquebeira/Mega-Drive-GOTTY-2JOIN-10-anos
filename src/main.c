#include <genesis.h>
#include "resources.h"

int main(bool hard)
{
    JOY_init();
    SPR_init();

    // Pinta a cor 0 do fundo de azul escuro
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x002266));

    // Escreve um texto na tela
    VDP_drawText("Teste de Sprite!", 12, 2);

    // Carrega a paleta do personagem na Paleta 1
    PAL_setPalette(PAL1, spr_idle.palette->data, DMA);

    // Posição inicial: centro horizontal (144) e parte inferior da tela (180)
    s16 x = 144;
    s16 y = 180;
    bool andando = FALSE;

    // Cria o sprite começando parado (idle) e olhando para a direita (TRUE)
    Sprite* player = SPR_addSprite(&spr_idle, x, y, TILE_ATTR(PAL1, FALSE, FALSE, TRUE));

    while(TRUE)
    {
        u16 controle = JOY_readJoypad(JOY_1);
        bool movendoAgora = FALSE;

        // Movimento apenas para esquerda e direita
        if (controle & BUTTON_LEFT)  { x -= 2; SPR_setHFlip(player, FALSE); movendoAgora = TRUE; }
        if (controle & BUTTON_RIGHT) { x += 2; SPR_setHFlip(player, TRUE);  movendoAgora = TRUE; }

        // Troca entre parado (idle) e correndo (run)
        if (movendoAgora && !andando)
        {
            SPR_setDefinition(player, &spr_run);
            andando = TRUE;
        }
        else if (!movendoAgora && andando)
        {
            SPR_setDefinition(player, &spr_idle);
            andando = FALSE;
        }

        // Limites laterais da tela (320px - 32px do sprite = 288)
        if (x < 0)   x = 0;
        if (x > 288) x = 288;

        SPR_setPosition(player, x, y);
        SPR_update();

        SYS_doVBlankProcess();
    }

    return 0;
}
