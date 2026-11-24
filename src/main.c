#include <genesis.h>
#include "resources.h" // Arquivo gerado automaticamente a partir de res/resources.res

int main(bool hard)
{
    JOY_init();
    SPR_init();

    // Carrega as 16 cores do sprite na Paleta 1 (PAL1)
    PAL_setPalette(PAL1, spr_player.palette->data, DMA);

    // Posição inicial em pixels (centro da tela de 320x224)
    s16 x = 152;
    s16 y = 104;

    // Cria o sprite na tela usando a Paleta 1
    Sprite* player = SPR_addSprite(&spr_player, x, y, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));

    while(TRUE)
    {
        u16 controle = JOY_readJoypad(JOY_1);

        // Move 2 pixels por frame e espelha o sprite ao virar para a esquerda/direita
        if (controle & BUTTON_LEFT)  { x -= 2; SPR_setHFlip(player, TRUE); }
        if (controle & BUTTON_RIGHT) { x += 2; SPR_setHFlip(player, FALSE); }
        if (controle & BUTTON_UP)    { y -= 2; }
        if (controle & BUTTON_DOWN)  { y += 2; }

        // Impede o sprite de sair das bordas da tela (320 - 16 = 304 | 224 - 16 = 208)
        if (x < 0)   x = 0;
        if (x > 304) x = 304;
        if (y < 0)   y = 0;
        if (y > 208) y = 208;

        // Atualiza a posição e desenha todos os sprites na tela
        SPR_setPosition(player, x, y);
        SPR_update();

        SYS_doVBlankProcess();
    }

    return 0;
}
