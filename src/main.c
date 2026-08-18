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

// Define enemy data here (only once)
Vector2 enemyPositions[10] = { {2, 2} };
int enemyCount = 1;

// UI panel constants
#define PANEL_HEIGHT 80
#define PANEL_Y (SCREEN_HEIGHT - PANEL_HEIGHT)
#define BUTTON_WIDTH 120
#define BUTTON_HEIGHT 50
#define BUTTON_SPACING 20
#define BUTTON_Y (PANEL_Y + (PANEL_HEIGHT - BUTTON_HEIGHT) / 2)

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Isometric RPG - Enemy Hover");
    SetTargetFPS(60);

    TileMap tilemap = CreateTileMap(MAP_SIZE, MAP_SIZE, TILE_SIZE);
    GenerateTestMap(&tilemap);

    Player player = CreatePlayer();
    player.worldPos = (Vector3){ MAP_SIZE/2.0f, 0, MAP_SIZE/2.0f };
    player.targetPos = player.worldPos;

    int reachable[100][2], reachableCount;
    int hoverX = -1, hoverZ = -1;
    bool hoverValid = false;
    bool meleeHover = false, rangedHover = false, skillHover = false;
    bool enemyHover = false;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        UpdatePlayer(&player, &tilemap, dt);

        GetReachableTiles(&player, &tilemap, reachable, &reachableCount);

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

        // Check enemy hover
        enemyHover = false;
        if (clickedX == (int)enemyPositions[0].x && clickedZ == (int)enemyPositions[0].y) {
            enemyHover = true;
        }

        hoverValid = false;
        if (!player.isMoving && clickedX >= 0 && clickedX < MAP_SIZE && clickedZ >= 0 && clickedZ < MAP_SIZE) {
            if (IsTileReachable(&player, &tilemap, clickedX, clickedZ)) {
                hoverValid = true;
                hoverX = clickedX;
                hoverZ = clickedZ;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    MovePlayerToTile(&player, clickedX, clickedZ);
                }
            }
        }

        // UI button detection
        meleeHover = rangedHover = skillHover = false;
        if (mouse.y >= PANEL_Y) {
            int btnStartX = (SCREEN_WIDTH - (3 * BUTTON_WIDTH + 2 * BUTTON_SPACING)) / 2;
            Rectangle meleeRect = { btnStartX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT };
            if (CheckCollisionPointRec(mouse, meleeRect)) meleeHover = true;
            Rectangle rangedRect = { btnStartX + BUTTON_WIDTH + BUTTON_SPACING, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT };
            if (CheckCollisionPointRec(mouse, rangedRect)) rangedHover = true;
            Rectangle skillRect = { btnStartX + 2 * (BUTTON_WIDTH + BUTTON_SPACING), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT };
            if (CheckCollisionPointRec(mouse, skillRect)) skillHover = true;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (meleeHover) printf("Melee clicked\n");
                else if (rangedHover) printf("Ranged clicked\n");
                else if (skillHover) printf("Skill clicked\n");
            }
        }

        BeginDrawing();
        ClearBackground((Color){30,30,40,255});

        // ---- Draw game world ----
        DrawTileMap(&tilemap, viewOffset);

        // ---- Draw enemy (red normally, orange on hover) ----
        Vector3 enemyPos3 = { enemyPositions[0].x, 0, enemyPositions[0].y };
        Vector2 enemyScreen = WorldToScreen(enemyPos3, viewOffset, TILE_SIZE);
        Vector2 enemyShadowScreen = WorldToScreen((Vector3){enemyPositions[0].x, 0, enemyPositions[0].y}, viewOffset, TILE_SIZE);
        DrawEllipse(enemyShadowScreen.x, enemyShadowScreen.y, 16, 8, (Color){0,0,0,80});
        
        Color enemyColor = enemyHover ? ORANGE : RED;
        Color enemyLine = enemyHover ? (Color){255, 200, 0, 255} : (Color){200, 0, 0, 255};
        DrawCircleV(enemyScreen, 16, enemyColor);
        DrawCircleLines(enemyScreen.x, enemyScreen.y, 16, enemyLine);
        DrawCircle(enemyScreen.x, enemyScreen.y, 3, (Color){255,255,255,200});
        DrawText("E", enemyScreen.x - 5, enemyScreen.y - 8, 16, WHITE);

        // ---- Red X on enemy tile (always) ----
        int s = 20;
        DrawLine(enemyScreen.x - s, enemyScreen.y - s, enemyScreen.x + s, enemyScreen.y + s, (Color){255,0,0,180});
        DrawLine(enemyScreen.x + s, enemyScreen.y - s, enemyScreen.x - s, enemyScreen.y + s, (Color){255,0,0,180});

        // ---- Orange highlights for reachable tiles ----
        if (!player.isMoving) {
            for (int i = 0; i < reachableCount; i++) {
                Vector3 pos = { (float)reachable[i][0], 0, (float)reachable[i][1] };
                Vector2 screenPos = WorldToScreen(pos, viewOffset, TILE_SIZE);
                DrawIsometricDiamond(screenPos, TILE_SIZE, (Color){255, 165, 0, 130});
                int halfW = TILE_SIZE/2, halfH = TILE_SIZE/4;
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

        // ---- Hover highlight ----
        if (hoverValid) {
            Vector3 pos = { (float)hoverX, 0, (float)hoverZ };
            Vector2 screenPos = WorldToScreen(pos, viewOffset, TILE_SIZE);
            DrawIsometricDiamond(screenPos, TILE_SIZE, (Color){255, 200, 50, 180});
            DrawCircleLines(screenPos.x, screenPos.y, 24, (Color){255,200,0,255});
            DrawCircleLines(screenPos.x, screenPos.y, 28, (Color){255,200,0,100});
        }

        DrawPlayer(&player, TILE_SIZE, viewOffset);

        // ---- UI Panel ----
        DrawRectangle(0, PANEL_Y, SCREEN_WIDTH, PANEL_HEIGHT, (Color){40, 40, 50, 220});
        DrawLine(0, PANEL_Y, SCREEN_WIDTH, PANEL_Y, (Color){80, 80, 100, 255});

        int btnStartX = (SCREEN_WIDTH - (3 * BUTTON_WIDTH + 2 * BUTTON_SPACING)) / 2;
        DrawRectangle(btnStartX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, meleeHover ? (Color){220,220,240,255} : (Color){70,70,90,255});
        DrawRectangleLines(btnStartX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, (Color){150,150,170,255});
        DrawText("Melee", btnStartX + 30, BUTTON_Y + 15, 20, WHITE);

        int rangedX = btnStartX + BUTTON_WIDTH + BUTTON_SPACING;
        DrawRectangle(rangedX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, rangedHover ? (Color){220,220,240,255} : (Color){70,70,90,255});
        DrawRectangleLines(rangedX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, (Color){150,150,170,255});
        DrawText("Ranged", rangedX + 25, BUTTON_Y + 15, 20, WHITE);

        int skillX = rangedX + BUTTON_WIDTH + BUTTON_SPACING;
        DrawRectangle(skillX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, skillHover ? (Color){220,220,240,255} : (Color){70,70,90,255});
        DrawRectangleLines(skillX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, (Color){150,150,170,255});
        DrawText("Skill", skillX + 35, BUTTON_Y + 15, 20, WHITE);

        // Top-left info
        DrawText("ISOMETRIC RPG - Hover enemy to see orange", 10, 10, 20, WHITE);
        DrawText("Arrow keys / click orange tiles to move", 10, 40, 16, LIGHTGRAY);
        char pos[100];
        snprintf(pos, sizeof(pos), "Tile: (%d, %d)", (int)roundf(player.worldPos.x), (int)roundf(player.worldPos.z));
        DrawText(pos, 10, 70, 16, LIGHTGRAY);
        if (player.isMoving) DrawText("Moving...", 10, 95, 16, YELLOW);

        EndDrawing();
    }

    UnloadTileMap(&tilemap);
    CloseWindow();
    return 0;
}