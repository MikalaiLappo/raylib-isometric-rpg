#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "isometric.h"
#include "player.h"
#include "tilemap.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define TILE_SIZE 64
#define MAP_SIZE 10

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Isometric RPG - Step 3");
    SetTargetFPS(60);

    TileMap tilemap = CreateTileMap(MAP_SIZE, MAP_SIZE, TILE_SIZE);
    GenerateTestMap(&tilemap);

    Player player = CreatePlayer();
    player.worldPos = (Vector3){ MAP_SIZE/2.0f, 0, MAP_SIZE/2.0f };
    player.targetPos = player.worldPos;

    // Reachable tiles storage
    int reachable[100][2];
    int reachableCount = 0;

    int hoverX = -1, hoverZ = -1;
    bool hoverValid = false;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        UpdatePlayer(&player, &tilemap, dt);

        // Get reachable tiles
        GetReachableTiles(&player, &tilemap, reachable, &reachableCount);

        // Camera offset to center player
        Vector2 playerScreen = WorldToScreen(player.worldPos, (Vector2){0,0}, TILE_SIZE);
        Vector2 viewOffset = {
            SCREEN_WIDTH/2.0f - playerScreen.x,
            SCREEN_HEIGHT/2.5f - playerScreen.y
        };

        // Mouse handling
        Vector2 mouse = GetMousePosition();
        Vector2 mouseWorld = ScreenToWorld(
            (Vector2){ mouse.x - viewOffset.x, mouse.y - viewOffset.y },
            TILE_SIZE
        );
        int clickedX = (int)roundf(mouseWorld.x);
        int clickedZ = (int)roundf(mouseWorld.y);

        hoverValid = false;
        if (!player.isMoving && clickedX >= 0 && clickedX < MAP_SIZE && clickedZ >= 0 && clickedZ < MAP_SIZE) {
            // Check if tile is reachable
            if (IsTileReachable(&player, &tilemap, clickedX, clickedZ)) {
                hoverValid = true;
                hoverX = clickedX;
                hoverZ = clickedZ;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    MovePlayerToTile(&player, clickedX, clickedZ);
                }
            }
        }

        BeginDrawing();
        ClearBackground((Color){30,30,40,255});

        // Draw tilemap
        DrawTileMap(&tilemap, viewOffset);

        // Draw reachable tiles with orange floor (full tile overlay)
        if (!player.isMoving) {
            for (int i = 0; i < reachableCount; i++) {
                Vector3 pos = { (float)reachable[i][0], 0, (float)reachable[i][1] };
                Vector2 screenPos = WorldToScreen(pos, viewOffset, TILE_SIZE);
                // Draw orange diamond with alpha
                DrawIsometricDiamond(screenPos, TILE_SIZE, (Color){255, 165, 0, 130});
                // Thick outline
                int halfW = TILE_SIZE/2;
                int halfH = TILE_SIZE/4;
                Vector2 top = { screenPos.x, screenPos.y - halfH };
                Vector2 right = { screenPos.x + halfW, screenPos.y };
                Vector2 bottom = { screenPos.x, screenPos.y + halfH };
                Vector2 left = { screenPos.x - halfW, screenPos.y };
                Color outline = {255, 165, 0, 200};
                DrawLineEx(top, right, 2.0f, outline);
                DrawLineEx(right, bottom, 2.0f, outline);
                DrawLineEx(bottom, left, 2.0f, outline);
                DrawLineEx(left, top, 2.0f, outline);
            }
        }

        // Draw hover highlight (brighter)
        if (hoverValid) {
            Vector3 pos = { (float)hoverX, 0, (float)hoverZ };
            Vector2 screenPos = WorldToScreen(pos, viewOffset, TILE_SIZE);
            // Bright orange diamond
            DrawIsometricDiamond(screenPos, TILE_SIZE, (Color){255, 200, 50, 180});
            // Extra rings
            DrawCircleLines(screenPos.x, screenPos.y, 24, (Color){255,200,0,255});
            DrawCircleLines(screenPos.x, screenPos.y, 28, (Color){255,200,0,100});
        }

        // Draw player
        DrawPlayer(&player, TILE_SIZE, viewOffset);

        EndDrawing();

        // UI (screen space)
        DrawText("ISOMETRIC RPG - Step 3 tiles", 10, 10, 20, WHITE);
        DrawText("Click on orange tiles or use arrow keys", 10, 40, 16, LIGHTGRAY);
        char pos[100];
        snprintf(pos, sizeof(pos), "Tile: (%d, %d)", (int)roundf(player.worldPos.x), (int)roundf(player.worldPos.z));
        DrawText(pos, 10, 70, 16, LIGHTGRAY);
        if (player.isMoving) {
            DrawText("Moving...", 10, 95, 16, YELLOW);
        }
    }

    UnloadTileMap(&tilemap);
    CloseWindow();
    return 0;
}