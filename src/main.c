#include <genesis.h>

int main(bool hard)
{
    // Inicializa o sistema de leitura dos controles
    JOY_init();

    // Posição inicial em tiles (a tela do Mega Drive tem 40x28 tiles)
    s16 x = 18;
    s16 y = 13;
    u16 timer = 0;

    VDP_drawText("Use as setas e A/B/C", 10, 2);
    VDP_drawText("<o>", x, y);

    while(TRUE)
    {
        // Lê o estado atual do Controle 1
        u16 controle = JOY_readJoypad(JOY_1);

        // Controla a velocidade (move a cada 4 frames para não correr rápido demais)
        if (timer > 0)
        {
            timer--;
        }
        else
        {
            bool moveu = FALSE;
            s16 novoX = x;
            s16 novoY = y;

            if (controle & BUTTON_LEFT)  { novoX--; moveu = TRUE; }
            if (controle & BUTTON_RIGHT) { novoX++; moveu = TRUE; }
            if (controle & BUTTON_UP)    { novoY--; moveu = TRUE; }
            if (controle & BUTTON_DOWN)  { novoY++; moveu = TRUE; }

            // Limita o movimento dentro das bordas da tela (40 colunas x 28 linhas)
            if (novoX < 0)  novoX = 0;
            if (novoX > 37) novoX = 37; // 40 - 3 caracteres de "<o>"
            if (novoY < 4)  novoY = 4;  // Evita apagar o texto do topo
            if (novoY > 27) novoY = 27;

            if (moveu)
            {
                // Apaga a posição antiga antes de desenhar na nova
                VDP_clearTextArea(x, y, 3, 1);
                x = novoX;
                y = novoY;
                VDP_drawText("<o>", x, y);
                timer = 4;
            }
        }

        // Exemplo de leitura dos botões de ação
        if (controle & BUTTON_A) VDP_drawText("Botao A!", 16, 25);
        if (controle & BUTTON_B) VDP_drawText("Botao B!", 16, 25);
        if (controle & BUTTON_C) VDP_clearTextArea(16, 25, 10, 1);

        SYS_doVBlankProcess();
    }

    return 0;
}
