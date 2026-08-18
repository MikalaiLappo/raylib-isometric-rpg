#include <math.h>
#include <stdlib.h>
#include "player.h"
#include "game/enemy.h"          // for IsTileOccupiedByEnemy
#include "rendering/isometric.h"
#include "rendering/tilemap.h"

Player CreatePlayer(void) {
    Player player;
    player.worldPos = (Vector3){ 0, 0, 0 };
    player.targetPos = (Vector3){ 0, 0, 0 };
    player.pathLength = 0;
    player.pathIndex = 0;
    player.isMoving = false;
    player.stepTimer = 0.0f;
    player.stepDuration = 0.12f;
    player.color = GREEN;
    return player;
}

bool IsTileOccupied(int tx, int tz) {
    return IsTileOccupiedByEnemy(tx, tz);
}

static void ComputePath(Player* player, int tx, int tz) {
    int cx = (int)roundf(player->worldPos.x);
    int cz = (int)roundf(player->worldPos.z);
    int dx = (tx > cx) ? 1 : (tx < cx) ? -1 : 0;
    int dz = (tz > cz) ? 1 : (tz < cz) ? -1 : 0;
    int steps = 0;
    bool blocked = false;
    while ((cx != tx || cz != tz) && steps < 16) {
        if (cx != tx) cx += dx;
        else if (cz != tz) cz += dz;
        if (IsTileOccupied(cx, cz)) {
            blocked = true;
            break;
        }
        player->pathX[steps] = cx;
        player->pathZ[steps] = cz;
        steps++;
    }
    if (blocked) player->pathLength = 0;
    else player->pathLength = steps;
    player->pathIndex = 0;
}

void MovePlayerToTile(Player* player, int tx, int tz) {
    if (player->isMoving) return;
    if (IsTileOccupied(tx, tz)) return;
    ComputePath(player, tx, tz);
    if (player->pathLength == 0) return;
    player->targetPos = (Vector3){ (float)tx, 0, (float)tz };
    player->isMoving = true;
    player->stepTimer = 0.0f;
}

void GetReachableTiles(Player* player, TileMap* map, int reachable[][2], int* count) {
    *count = 0;
    if (player->isMoving) return;
    int cx = (int)roundf(player->worldPos.x);
    int cz = (int)roundf(player->worldPos.z);
    for (int dz = -MAX_STEPS; dz <= MAX_STEPS; dz++) {
        for (int dx = -MAX_STEPS; dx <= MAX_STEPS; dx++) {
            int dist = abs(dx) + abs(dz);
            if (dist == 0 || dist > MAX_STEPS) continue;
            int nx = cx + dx, nz = cz + dz;
            if (nx < 0 || nx >= map->width || nz < 0 || nz >= map->height) continue;
            if (IsTileOccupied(nx, nz)) continue;
            // Check if path would be blocked
            bool blocked = false;
            int tempX = cx, tempZ = cz;
            int stepX = (nx > cx) ? 1 : (nx < cx) ? -1 : 0;
            int stepZ = (nz > cz) ? 1 : (nz < cz) ? -1 : 0;
            while (tempX != nx && !blocked) {
                tempX += stepX;
                if (IsTileOccupied(tempX, cz)) blocked = true;
            }
            while (tempZ != nz && !blocked) {
                tempZ += stepZ;
                if (IsTileOccupied(nx, tempZ)) blocked = true;
            }
            if (!blocked) {
                reachable[*count][0] = nx;
                reachable[*count][1] = nz;
                (*count)++;
            }
        }
    }
}

bool IsTileReachable(Player* player, TileMap* map, int tx, int tz) {
    int reachable[100][2], count;
    GetReachableTiles(player, map, reachable, &count);
    for (int i = 0; i < count; i++) {
        if (reachable[i][0] == tx && reachable[i][1] == tz) return true;
    }
    return false;
}

void UpdatePlayer(Player* player, TileMap* map, float dt) {
    if (!player->isMoving) {
        int cx = (int)roundf(player->worldPos.x);
        int cz = (int)roundf(player->worldPos.z);
        int nx = cx, nz = cz;
        bool moved = false;
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) { nz--; moved = true; }
        else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) { nz++; moved = true; }
        else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) { nx--; moved = true; }
        else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) { nx++; moved = true; }
        if (moved && nx >= 0 && nx < map->width && nz >= 0 && nz < map->height) {
            if (!IsTileOccupied(nx, nz)) MovePlayerToTile(player, nx, nz);
        }
    }
    if (player->isMoving) {
        player->stepTimer += dt;
        if (player->stepTimer >= player->stepDuration) {
            player->stepTimer = 0.0f;
            if (player->pathIndex < player->pathLength) {
                int tx = player->pathX[player->pathIndex];
                int tz = player->pathZ[player->pathIndex];
                if (!IsTileOccupied(tx, tz)) {
                    player->worldPos.x = (float)tx;
                    player->worldPos.z = (float)tz;
                    player->pathIndex++;
                    if (player->pathIndex >= player->pathLength) {
                        player->worldPos = player->targetPos;
                        player->isMoving = false;
                    }
                } else {
                    player->isMoving = false;
                }
            } else {
                player->isMoving = false;
            }
        }
    }
}

void DrawPlayer(Player* player, int tileSize, Vector2 viewOffset) {
    Vector2 screenPos = WorldToScreen(player->worldPos, viewOffset, tileSize);
    Vector3 shadowPos = { player->worldPos.x, 0, player->worldPos.z };
    Vector2 shadowScreen = WorldToScreen(shadowPos, viewOffset, tileSize);
    DrawEllipse(shadowScreen.x, shadowScreen.y, 16, 8, (Color){0,0,0,80});
    DrawCircleV(screenPos, 16, player->color);
    DrawCircleLines(screenPos.x, screenPos.y, 16, (Color){0,200,0,255});
    DrawCircle(screenPos.x, screenPos.y, 3, (Color){255,255,255,255});
}