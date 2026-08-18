#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "rendering/tilemap.h"

#define MAX_ENEMIES 10

typedef struct {
    Vector2 pos;
    bool    alive;
    Color   color;
    int     health;
    int     maxHealth;
    float   hitFlashTimer;
} Enemy;

extern Enemy enemies[MAX_ENEMIES];
extern int   enemyCount;

void InitEnemies(void);
void SpawnEnemy(const TileMap* map, Vector2 avoidPos);
void TakeDamage(int index, int damage);
void DrawEnemies(Vector2 viewOffset, int tileSize);
bool IsTileOccupiedByEnemy(int tx, int tz);
int  GetEnemyAtTile(int tx, int tz);

#endif