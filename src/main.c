#include <genesis.h>

int main(bool hard)
{
    // Escreve o texto na tela na posição X = 12, Y = 13 (em tiles de 8x8 pixels)
    VDP_drawText("Hello Mega Drive!", 12, 13);

    // Loop principal do jogo
    while(TRUE)
    {
        // Atualiza o processamento de vídeo a cada frame (60 FPS em NTSC / 50 FPS em PAL)
        SYS_doVBlankProcess();
    }

    return 0;
}
