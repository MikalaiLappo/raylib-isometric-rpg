#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "rendering/isometric.h"
#include "rendering/tilemap.h"
#include <stdbool.h>

#define MAX_STEPS 3

typedef struct {
    Vector3 worldPos;
    Vector3 targetPos;
    int     pathX[16];
    int     pathZ[16];
    int     pathLength;
    int     pathIndex;
    bool    isMoving;
    float   stepTimer;
    float   stepDuration;
    Color   color;
} Player;

Player CreatePlayer(void);
void   UpdatePlayer(Player* player, const TileMap* map, float dt);
void   MovePlayerToTile(Player* player, int tx, int tz);
void   GetReachableTiles(const Player* player, const TileMap* map, int reachable[][2], int* count);
bool   IsTileReachable(const Player* player, const TileMap* map, int tx, int tz);
bool   IsTileOccupied(int tx, int tz);
void   DrawPlayer(const Player* player, int tileSize, Vector2 viewOffset);
#endif