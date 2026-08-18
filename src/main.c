#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "game/game.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Isometric RPG");
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