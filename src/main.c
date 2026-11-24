#include <genesis.h>
#include "resources.h"

int main(bool hard)
{
    JOY_init();
    SPR_init();

    // Pinta a cor 0 do fundo de azul escuro (para enxergar sprites pretos/escuros)
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x002266));

    // Escreve um texto na tela para confirmar que a ROM iniciou
    VDP_drawText("Teste de Sprite!", 12, 2);

    // Carrega a paleta do sprite na Paleta 1
    PAL_setPalette(PAL1, spr_player.palette->data, DMA);

    s16 x = 144;
    s16 y = 96;

    Sprite* player = SPR_addSprite(&spr_player, x, y, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));

    while(TRUE)
    {
        u16 controle = JOY_readJoypad(JOY_1);

        if (controle & BUTTON_LEFT)  { x -= 2; SPR_setHFlip(player, TRUE); }
        if (controle & BUTTON_RIGHT) { x += 2; SPR_setHFlip(player, FALSE); }
        if (controle & BUTTON_UP)    { y -= 2; }
        if (controle & BUTTON_DOWN)  { y += 2; }

        if (x < 0)   x = 0;
        if (x > 288) x = 288;
        if (y < 0)   y = 0;
        if (y > 192) y = 192;

        SPR_setPosition(player, x, y);
        SPR_update();

        SYS_doVBlankProcess();
    }

    return 0;
}
