#ifndef JOGO_H
#define JOGO_H

#include <genesis.h>
#include "resources.h"

#define MAX_TIROS 4
#define MAX_TIROS_INIMIGO 12
#define SFX_TIRO_ID 64
#define BGM_MENU_ID 65
#define BGM_JOGO_ID 66
#define SFX_TOASTY_ID 67
#define SFX_DANO_ID 68
#define SFX_EXPLOSAO_ID 69

#define TIPO_N64 0
#define TIPO_ARANHA 1
#define TIPO_DISJUNTOR 2
#define TIPO_PNEU 3

#define MAX_N64 3
#define MAX_ARANHA 2
#define MAX_DISJUNTOR 1

#define MAX_ESTILHACOS 4

typedef struct {
    Sprite* sprite;
    s16 x;
    s16 y;
    bool andando;
    bool olhandoDireita;
    u8 cooldownTiro;
    u16 timerPiscaDano;
// Adicione dentro de typedef struct { ... } Player; no src/jogo.h:
    u8 multiplicador;     // 1 a 8
    u8 killsNoNivel;      // Quantos inimigos derrotou no nível atual do combo
    u8 metaProximoNivel;  // 4 para 2x, 6 para 3x, 8 para 4x...
    Sprite* spriteMult;   // Ícone 2X..8X acima da barra
    Sprite* spriteToasty;
    u8 timerToasty;       // Controla a subida e descida no canto inferior direito
} Player;

typedef struct {
    Sprite* sprite;
    s16 x;
    s16 y;
    s16 vx;
    s16 vy;
    bool ativo;
} Tiro;

typedef struct {
    Sprite* sprite;
    s16 x;
    s16 y;
    s16 baseX;
    s16 baseY;
    s8 vx;
    s8 vy;
    u8 tipo;
    u8 largura;
    u8 altura;
    u8 hp;
    bool ativo;
    u16 timerRespawn;
    u16 timerTiro;
    u16 timerPisca;
} Inimigo;

typedef struct {
    Sprite* sprite;
    s16 x;
    s16 y;
    s8 vx;
    s8 vy;
} Estilhaco;

// Funções de telas e HUD (src/telas.c)
void inicializarFonteHUD(void);
void atualizarPlacar(u32 pontos);
void atualizarContadorEdicao(u16 edicao);
void executarTelaInicial(void);
void executarTelaFinal(u32);

// Funções do jogador e tiros (src/player.c)
void inicializarPlayer(Player* p, Tiro tiros[]);

// Funções de inimigos (src/inimigos.c)
void spawnN64(Inimigo* n64);
void spawnAranha(Inimigo* aranha);
void spawnDisjuntor(Inimigo* disjuntor, s16 playerX);
void spawnPneu(Inimigo* pneu);

void inicializarTilesBarraCombo(void);
void atualizarBarraCombo(Player* p);
void registrarAbateCombo(Player* p);
void resetarCombo(Player* p);

void atualizarAnimacaoToasty(Player* p);
HINTERRUPT_CALLBACK onHBlankToasty(void);

void tocarSomDanoPSG(void);
void atualizarSomDanoPSG(void);

void inicializarEstilhacos(Estilhaco estilhacos[]);
void dispararEstilhacos(Estilhaco estilhacos[], const SpriteDefinition* spriteDef, u8 paleta, s16 centroX, s16 centroY);
void atualizarEstilhacos(Estilhaco estilhacos[]);

void inicializarInimigos(Inimigo n64[], Inimigo aranha[], Inimigo disjuntor[], Inimigo* pneu, Tiro tirosInimigo[]);
void atualizarInimigosETiros(Inimigo n64[], Inimigo aranha[], Inimigo disjuntor[], Inimigo* pneu, Tiro tirosInimigo[], Player* player, u16 edicao);
void atualizarPlayerETiros(Player* p, Tiro tiros[], Inimigo n64[], Inimigo aranha[], Inimigo disjuntor[], Inimigo* pneu, Estilhaco estilhacos[], u32* pontos);

void verificarColisoesPlayerETiros(Player* p, Tiro tiros[], Inimigo n64[], Inimigo aranha[], Inimigo disjuntor[], Inimigo* pneu, Estilhaco estilhacos[], u32* pontos);

void iniciarMusicaSecretaJazz(void);
void atualizarMusicaSecretaJazz(void);
void iniciarMusicaSecretaSynth80(void);
void atualizarMusicaSecretaSynth80(void);
void pararMusicaSecreta(void);
void iniciarMusicaSecretaPSG(void);
void atualizarMusicaSecretaPSG(void);
void pararMusicaSecretaPSG(void);

#endif
