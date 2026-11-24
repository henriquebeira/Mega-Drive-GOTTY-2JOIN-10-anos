#include <genesis.h>
#include "resources.h"

#define MAX_TIROS 4

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

    // Pinta a cor 0 do fundo de azul escuro
    PAL_setColor(0, RGB24_TO_VDPCOLOR(0x002266));
    VDP_drawText("Teste de Sprite!", 12, 2);

    // Carrega a paleta do personagem na PAL1 e a do tiro na PAL2
    PAL_setPalette(PAL1, spr_idle.palette->data, DMA);
    PAL_setPalette(PAL2, spr_tiro.palette->data, DMA);

    s16 x = 144;
    s16 y = 180;
    bool andando = FALSE;
    bool olhandoDireita = TRUE;
    bool botaoTiroTravado = FALSE;

    // Personagem começa parado (idle) e olhando para a direita (TRUE)
    Sprite* player = SPR_addSprite(&spr_idle, x, y, TILE_ATTR(PAL1, FALSE, FALSE, TRUE));

    // Inicializa a lista de tiros
    Tiro tiros[MAX_TIROS];
    for (u8 i = 0; i < MAX_TIROS; i++) {
        tiros[i].ativo = FALSE;
        tiros[i].sprite = NULL;
    }

    while(TRUE)
    {
        u16 controle = JOY_readJoypad(JOY_1);
        bool movendoAgora = FALSE;

        // Movimento apenas para esquerda e direita
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

        // Troca entre parado (idle) e correndo (run)
        if (movendoAgora && !andando) {
            SPR_setDefinition(player, &spr_run);
            andando = TRUE;
        } else if (!movendoAgora && andando) {
            SPR_setDefinition(player, &spr_idle);
            andando = FALSE;
        }

        // Disparo para cima ao apertar o botão B (ou A)
        if (controle & (BUTTON_B | BUTTON_A)) {
            if (!botaoTiroTravado) {
                botaoTiroTravado = TRUE;

                for (u8 i = 0; i < MAX_TIROS; i++) {
                    if (!tiros[i].ativo) {
                        tiros[i].ativo = TRUE;
                        tiros[i].y = y - 2;  // Sai bem na boca superior do canhão
                        tiros[i].vy = -5;    // Sobe em direção ao topo da tela

                        if (olhandoDireita) {
                            tiros[i].x = x + 5;  // Canhão fica à esquerda quando olha para a direita
                        } else {
                            tiros[i].x = x + 19; // Canhão fica à direita quando olha para a esquerda
                        }

                        tiros[i].sprite = SPR_addSprite(&spr_tiro, tiros[i].x, tiros[i].y, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
                        break;
                    }
                }
            }
        } else {
            botaoTiroTravado = FALSE;
        }

        // Move os tiros para cima e apaga quando saem do topo da tela
        for (u8 i = 0; i < MAX_TIROS; i++) {
            if (tiros[i].ativo) {
                tiros[i].y += tiros[i].vy;

                if (tiros[i].y < -8) {
                    SPR_releaseSprite(tiros[i].sprite);
                    tiros[i].ativo = FALSE;
                } else {
                    SPR_setPosition(tiros[i].sprite, tiros[i].x, tiros[i].y);
                }
            }
        }

        // Limites laterais do personagem
        if (x < 0)   x = 0;
        if (x > 288) x = 288;

        SPR_setPosition(player, x, y);
        SPR_update();

        SYS_doVBlankProcess();
    }

    return 0;
}
