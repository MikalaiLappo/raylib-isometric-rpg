#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "raylib.h"
#include "isometric.h"
#include "player.h"
#include "tilemap.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define TILE_SIZE 64
#define MAP_SIZE 10
#define MAX_ATTACK_TILES 500

// Enemy data
Vector2 enemyPositions[10] = { {2, 2} };
int enemyCount = 1;

// UI panel constants
#define PANEL_HEIGHT 80
#define PANEL_Y (SCREEN_HEIGHT - PANEL_HEIGHT)
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 40
#define BUTTON_SPACING 15
#define BUTTON_Y (PANEL_Y + (PANEL_HEIGHT - BUTTON_HEIGHT) / 2)

typedef enum { IDLE, MELEE, RANGED, SPELL } GameState;

static void GetAttackRange(GameState state, Player* player, TileMap* map, int rangeTiles[][2], int* count) {
    *count = 0;
    int cx = (int)roundf(player->worldPos.x);
    int cz = (int)roundf(player->worldPos.z);
    int maxRange = 0;
    if (state == MELEE) maxRange = 1;
    else if (state == RANGED) maxRange = 3;
    else if (state == SPELL) maxRange = MAP_SIZE;

    for (int dz = -maxRange; dz <= maxRange; dz++) {
        for (int dx = -maxRange; dx <= maxRange; dx++) {
            int dist = abs(dx) + abs(dz);
            if (dist == 0) continue;
            if (state == MELEE && (dx != 0 && dz != 0)) continue;
            if (state == RANGED && dist > 3) continue;
            if (state == MELEE && (dist > 1 || (dx != 0 && dz != 0))) continue;
            int nx = cx + dx;
            int nz = cz + dz;
            if (nx < 0 || nx >= map->width || nz < 0 || nz >= map->height) continue;
            if (*count >= MAX_ATTACK_TILES - 1) break;
            rangeTiles[*count][0] = nx;
            rangeTiles[*count][1] = nz;
            (*count)++;
        }
        if (*count >= MAX_ATTACK_TILES - 1) break;
    }
}

static bool IsInAttackRange(GameState state, Player* player, TileMap* map, int tx, int tz) {
    int rangeTiles[MAX_ATTACK_TILES][2];
    int count;
    GetAttackRange(state, player, map, rangeTiles, &count);
    for (int i = 0; i < count; i++) {
        if (rangeTiles[i][0] == tx && rangeTiles[i][1] == tz) return true;
    }
    return false;
}

// Spawn enemy at random walkable tile (not player, not old enemy)
static void SpawnEnemy(Player* player, TileMap* map, Vector2* enemyPos) {
    int playerX = (int)roundf(player->worldPos.x);
    int playerZ = (int)roundf(player->worldPos.z);
    int oldX = (int)enemyPos->x;
    int oldZ = (int)enemyPos->y;
    // Collect all walkable tiles
    int candidateX[100], candidateZ[100];
    int count = 0;
    for (int z = 0; z < map->height; z++) {
        for (int x = 0; x < map->width; x++) {
            Tile* tile = GetTile(map, x, z);
            if (!tile) continue;
            // Skip player tile and old enemy tile
            if (x == playerX && z == playerZ) continue;
            if (x == oldX && z == oldZ) continue;
            // Skip tiles that are occupied by other enemies? We'll just keep one enemy.
            // Also skip water? We'll allow any tile.
            candidateX[count] = x;
            candidateZ[count] = z;
            count++;
        }
    }
    if (count == 0) {
        // No candidate, place at (1,1) as fallback
        enemyPos->x = 1;
        enemyPos->y = 1;
        return;
    }
    int idx = rand() % count;
    enemyPos->x = (float)candidateX[idx];
    enemyPos->y = (float)candidateZ[idx];
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Isometric RPG - Combat");
    SetTargetFPS(60);

    // Seed random
    srand((unsigned int)time(NULL));

    TileMap tilemap = CreateTileMap(MAP_SIZE, MAP_SIZE, TILE_SIZE);
    GenerateTestMap(&tilemap);

    Player player = CreatePlayer();
    player.worldPos = (Vector3){ MAP_SIZE/2.0f, 0, MAP_SIZE/2.0f };
    player.targetPos = player.worldPos;

    GameState state = IDLE;

    int reachable[100][2], reachableCount;
    int attackRange[MAX_ATTACK_TILES][2], attackCount = 0;

    int hoverX = -1, hoverZ = -1;
    bool hoverValid = false;
    bool enemyHover = false;

    bool meleeHover = false, rangedHover = false, spellHover = false, cancelHover = false;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        UpdatePlayer(&player, &tilemap, dt);

        Vector2 playerScreen = WorldToScreen(player.worldPos, (Vector2){0,0}, TILE_SIZE);
        Vector2 viewOffset = {
            SCREEN_WIDTH/2.0f - playerScreen.x,
            SCREEN_HEIGHT/2.5f - playerScreen.y
        };

        Vector2 mouse = GetMousePosition();
        Vector2 mouseWorld = ScreenToWorld(
            (Vector2){ mouse.x - viewOffset.x, mouse.y - viewOffset.y },
            TILE_SIZE
        );
        int mouseX = (int)roundf(mouseWorld.x);
        int mouseZ = (int)roundf(mouseWorld.y);

        hoverValid = false;
        enemyHover = false;

        // Check if mouse is over enemy
        if (enemyCount > 0) {
            Vector2 enemyPos = enemyPositions[0];
            if (mouseX == (int)enemyPos.x && mouseZ == (int)enemyPos.y) {
                enemyHover = true;
            }
        }

        // --- Handle mouse clicks ---
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (state == IDLE) {
                if (mouseX >= 0 && mouseX < MAP_SIZE && mouseZ >= 0 && mouseZ < MAP_SIZE) {
                    if (IsTileReachable(&player, &tilemap, mouseX, mouseZ)) {
                        MovePlayerToTile(&player, mouseX, mouseZ);
                    }
                }
            } else {
                // Attack mode: click enemy if in range
                if (enemyHover && enemyCount > 0) {
                    Vector2 enemyPos = enemyPositions[0];
                    if (IsInAttackRange(state, &player, &tilemap, (int)enemyPos.x, (int)enemyPos.y)) {
                        printf("Attacking enemy at (%d, %d) with %s!\n", (int)enemyPos.x, (int)enemyPos.y,
                               state == MELEE ? "Melee" : (state == RANGED ? "Ranged" : "Spell"));
                        // Kill enemy, spawn new one
                        enemyPositions[0] = (Vector2){ -1, -1 }; // remove
                        enemyCount = 0;
                        // Spawn new enemy
                        Vector2 newPos;
                        SpawnEnemy(&player, &tilemap, &newPos);
                        enemyPositions[0] = newPos;
                        enemyCount = 1;
                        printf("New enemy spawned at (%d, %d)\n", (int)newPos.x, (int)newPos.y);
                        // Optionally exit attack mode? We'll stay in mode.
                    } else {
                        printf("Enemy not in attack range\n");
                    }
                }
            }
        }

        // --- Update ranges ---
        if (state == IDLE) {
            GetReachableTiles(&player, &tilemap, reachable, &reachableCount);
        } else {
            GetAttackRange(state, &player, &tilemap, attackRange, &attackCount);
        }

        // --- Hover highlight ---
        if (state == IDLE) {
            if (mouseX >= 0 && mouseX < MAP_SIZE && mouseZ >= 0 && mouseZ < MAP_SIZE) {
                if (IsTileReachable(&player, &tilemap, mouseX, mouseZ)) {
                    hoverValid = true;
                    hoverX = mouseX;
                    hoverZ = mouseZ;
                }
            }
        } else {
            if (mouseX >= 0 && mouseX < MAP_SIZE && mouseZ >= 0 && mouseZ < MAP_SIZE) {
                if (IsInAttackRange(state, &player, &tilemap, mouseX, mouseZ)) {
                    hoverValid = true;
                    hoverX = mouseX;
                    hoverZ = mouseZ;
                }
            }
        }

        // --- UI button hover detection ---
        meleeHover = rangedHover = spellHover = cancelHover = false;
        if (mouse.y >= PANEL_Y) {
            if (state == IDLE) {
                int btnStartX = (SCREEN_WIDTH - (3 * BUTTON_WIDTH + 2 * BUTTON_SPACING)) / 2;
                Rectangle meleeRect = { btnStartX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT };
                if (CheckCollisionPointRec(mouse, meleeRect)) meleeHover = true;
                Rectangle rangedRect = { btnStartX + BUTTON_WIDTH + BUTTON_SPACING, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT };
                if (CheckCollisionPointRec(mouse, rangedRect)) rangedHover = true;
                Rectangle spellRect = { btnStartX + 2 * (BUTTON_WIDTH + BUTTON_SPACING), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT };
                if (CheckCollisionPointRec(mouse, spellRect)) spellHover = true;
            } else {
                int cancelX = (SCREEN_WIDTH - BUTTON_WIDTH) / 2;
                Rectangle cancelRect = { cancelX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT };
                if (CheckCollisionPointRec(mouse, cancelRect)) cancelHover = true;
            }
        }

        // --- Button click handling ---
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (state == IDLE && !player.isMoving) {
                int btnStartX = (SCREEN_WIDTH - (3 * BUTTON_WIDTH + 2 * BUTTON_SPACING)) / 2;
                Rectangle meleeRect = { btnStartX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT };
                if (CheckCollisionPointRec(mouse, meleeRect)) {
                    state = MELEE;
                    printf("Switched to Melee mode\n");
                }
                Rectangle rangedRect = { btnStartX + BUTTON_WIDTH + BUTTON_SPACING, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT };
                if (CheckCollisionPointRec(mouse, rangedRect)) {
                    state = RANGED;
                    printf("Switched to Ranged mode\n");
                }
                Rectangle spellRect = { btnStartX + 2 * (BUTTON_WIDTH + BUTTON_SPACING), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT };
                if (CheckCollisionPointRec(mouse, spellRect)) {
                    state = SPELL;
                    printf("Switched to Spell mode\n");
                }
            } else if (state != IDLE) {
                int cancelX = (SCREEN_WIDTH - BUTTON_WIDTH) / 2;
                Rectangle cancelRect = { cancelX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT };
                if (CheckCollisionPointRec(mouse, cancelRect)) {
                    state = IDLE;
                    printf("Canceled attack mode\n");
                }
            }
        }

        // --- Drawing ---
        BeginDrawing();
        ClearBackground((Color){30,30,40,255});

        DrawTileMap(&tilemap, viewOffset);

        // Draw enemy if exists
        if (enemyCount > 0) {
            Vector2 enemyPos = enemyPositions[0];
            Vector3 enemyPos3 = { enemyPos.x, 0, enemyPos.y };
            Vector2 enemyScreen = WorldToScreen(enemyPos3, viewOffset, TILE_SIZE);
            Vector2 enemyShadowScreen = WorldToScreen((Vector3){enemyPos.x, 0, enemyPos.y}, viewOffset, TILE_SIZE);
            DrawEllipse(enemyShadowScreen.x, enemyShadowScreen.y, 16, 8, (Color){0,0,0,80});
            Color enemyColor = enemyHover ? ORANGE : RED;
            Color enemyLine = enemyHover ? (Color){255,200,0,255} : (Color){200,0,0,255};
            DrawCircleV(enemyScreen, 16, enemyColor);
            DrawCircleLines(enemyScreen.x, enemyScreen.y, 16, enemyLine);
            DrawCircle(enemyScreen.x, enemyScreen.y, 3, (Color){255,255,255,200});
            DrawText("E", enemyScreen.x - 5, enemyScreen.y - 8, 16, WHITE);
            // Red X on enemy tile
            int s = 20;
            DrawLine(enemyScreen.x - s, enemyScreen.y - s, enemyScreen.x + s, enemyScreen.y + s, (Color){255,0,0,180});
            DrawLine(enemyScreen.x + s, enemyScreen.y - s, enemyScreen.x - s, enemyScreen.y + s, (Color){255,0,0,180});
        }

        // Highlight tiles
        if (state == IDLE) {
            for (int i = 0; i < reachableCount; i++) {
                Vector3 pos = { (float)reachable[i][0], 0, (float)reachable[i][1] };
                Vector2 screenPos = WorldToScreen(pos, viewOffset, TILE_SIZE);
                DrawIsometricDiamond(screenPos, TILE_SIZE, (Color){255,165,0,130});
                int halfW = TILE_SIZE/2, halfH = TILE_SIZE/4;
                Vector2 top = { screenPos.x, screenPos.y - halfH };
                Vector2 right = { screenPos.x + halfW, screenPos.y };
                Vector2 bottom = { screenPos.x, screenPos.y + halfH };
                Vector2 left = { screenPos.x - halfW, screenPos.y };
                Color outline = {255,165,0,200};
                DrawLineEx(top, right, 2.0f, outline);
                DrawLineEx(right, bottom, 2.0f, outline);
                DrawLineEx(bottom, left, 2.0f, outline);
                DrawLineEx(left, top, 2.0f, outline);
            }
        } else {
            for (int i = 0; i < attackCount; i++) {
                Vector3 pos = { (float)attackRange[i][0], 0, (float)attackRange[i][1] };
                Vector2 screenPos = WorldToScreen(pos, viewOffset, TILE_SIZE);
                DrawIsometricDiamond(screenPos, TILE_SIZE, (Color){255,0,0,130});
                int halfW = TILE_SIZE/2, halfH = TILE_SIZE/4;
                Vector2 top = { screenPos.x, screenPos.y - halfH };
                Vector2 right = { screenPos.x + halfW, screenPos.y };
                Vector2 bottom = { screenPos.x, screenPos.y + halfH };
                Vector2 left = { screenPos.x - halfW, screenPos.y };
                Color outline = {255,0,0,200};
                DrawLineEx(top, right, 2.0f, outline);
                DrawLineEx(right, bottom, 2.0f, outline);
                DrawLineEx(bottom, left, 2.0f, outline);
                DrawLineEx(left, top, 2.0f, outline);
            }
        }

        if (hoverValid) {
            Vector3 pos = { (float)hoverX, 0, (float)hoverZ };
            Vector2 screenPos = WorldToScreen(pos, viewOffset, TILE_SIZE);
            Color hoverColor = (state == IDLE) ? (Color){255,200,50,180} : (Color){255,100,100,180};
            DrawIsometricDiamond(screenPos, TILE_SIZE, hoverColor);
            DrawCircleLines(screenPos.x, screenPos.y, 24, (Color){255,200,0,255});
            DrawCircleLines(screenPos.x, screenPos.y, 28, (Color){255,200,0,100});
        }

        DrawPlayer(&player, TILE_SIZE, viewOffset);

        // UI Panel
        DrawRectangle(0, PANEL_Y, SCREEN_WIDTH, PANEL_HEIGHT, (Color){40,40,50,220});
        DrawLine(0, PANEL_Y, SCREEN_WIDTH, PANEL_Y, (Color){80,80,100,255});

        if (state == IDLE) {
            int btnStartX = (SCREEN_WIDTH - (3 * BUTTON_WIDTH + 2 * BUTTON_SPACING)) / 2;
            DrawRectangle(btnStartX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, meleeHover ? (Color){220,220,240,255} : (Color){70,70,90,255});
            DrawRectangleLines(btnStartX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, (Color){150,150,170,255});
            DrawText("Melee", btnStartX + 25, BUTTON_Y + 10, 18, WHITE);

            int rangedX = btnStartX + BUTTON_WIDTH + BUTTON_SPACING;
            DrawRectangle(rangedX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, rangedHover ? (Color){220,220,240,255} : (Color){70,70,90,255});
            DrawRectangleLines(rangedX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, (Color){150,150,170,255});
            DrawText("Ranged", rangedX + 20, BUTTON_Y + 10, 18, WHITE);

            int spellX = rangedX + BUTTON_WIDTH + BUTTON_SPACING;
            DrawRectangle(spellX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, spellHover ? (Color){220,220,240,255} : (Color){70,70,90,255});
            DrawRectangleLines(spellX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, (Color){150,150,170,255});
            DrawText("Spell", spellX + 25, BUTTON_Y + 10, 18, WHITE);
        } else {
            int cancelX = (SCREEN_WIDTH - BUTTON_WIDTH) / 2;
            DrawRectangle(cancelX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, cancelHover ? (Color){255,200,200,255} : (Color){200,70,70,255});
            DrawRectangleLines(cancelX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, (Color){255,150,150,255});
            DrawText("X", cancelX + 40, BUTTON_Y + 8, 24, WHITE);
            char modeText[20];
            snprintf(modeText, sizeof(modeText), "Mode: %s", state == MELEE ? "Melee" : (state == RANGED ? "Ranged" : "Spell"));
            DrawText(modeText, 10, PANEL_Y + 10, 18, YELLOW);
        }

        DrawText("ISOMETRIC RPG - Combat", 10, 10, 20, WHITE);
        if (state == IDLE) {
            DrawText("Click orange tiles to move, or choose attack mode", 10, 40, 16, LIGHTGRAY);
        } else {
            DrawText("Click on enemy (within red range) to attack, X to cancel", 10, 40, 16, LIGHTGRAY);
        }
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