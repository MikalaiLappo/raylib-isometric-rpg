#define _XOPEN_SOURCE 600
#include "game/game.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Isometric RPG");
    SetWindowState(FLAG_WINDOW_TOPMOST);

    SetTargetFPS(60);

    Game game;
    InitGame(&game);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        UpdateGame(&game, dt);
        DrawGame(&game);
    }

    UnloadTileMap(&game.tilemap);
    CloseWindow();
    return 0;
}