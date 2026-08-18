#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "rendering/isometric.h"
#include "rendering/tilemap.h"
#include <stdbool.h>

#define MAX_STEPS 3

extern Vector2 enemyPositions[10];
extern int enemyCount;

typedef struct {
    Vector3 worldPos;
    Vector3 targetPos;
    int pathX[16];
    int pathZ[16];
    int pathLength;
    int pathIndex;
    bool isMoving;
    float stepTimer;
    float stepDuration;
    Color color;
} Player;

Player CreatePlayer(void);
void UpdatePlayer(Player* player, TileMap* map, float dt);
void DrawPlayer(Player* player, int tileSize, Vector2 viewOffset);
void MovePlayerToTile(Player* player, int tx, int tz);
void GetReachableTiles(Player* player, TileMap* map, int reachable[][2], int* count);
bool IsTileReachable(Player* player, TileMap* map, int tx, int tz);
bool IsTileOccupied(int tx, int tz);

#endif