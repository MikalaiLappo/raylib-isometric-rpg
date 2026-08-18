#ifndef GAME_H
#define GAME_H

#include "game/animation.h" // <-- ADD THIS
#include "game/game_types.h"
#include "player.h"
#include "raylib.h"
#include "rendering/tilemap.h"
#include "ui.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define TILE_SIZE 64
#define MAP_SIZE 10
#define MAX_FLOATING_TEXTS 20

typedef struct {
    Vector2 screenPos;
    char    text[16];
    float   timer;
    float   maxTimer;
    Color   color;
} FloatingText;

typedef struct {
    GameState    state;
    Player       player;
    TileMap      tilemap;
    UIState      ui;
    Animation    animations[MAX_ANIMATIONS]; // <-- ADD THIS
    int          reachable[100][2];
    int          reachableCount;
    int          attackRange[500][2];
    int          attackCount;
    int          hoverX, hoverZ;
    bool         hoverValid;
    int          hoverEnemyIdx;
    bool         enemyHover;
    Vector2      viewOffset;
    FloatingText floatingTexts[MAX_FLOATING_TEXTS];
    int          floatingTextCount;
} Game;

void InitGame(Game* game);
void UpdateGame(Game* game, float dt);
void DrawGame(Game* game);
void AddFloatingText(Game* game, Vector2 screenPos, const char* text, Color color);

#endif