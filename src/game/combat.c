#include "game/combat.h"
#include "game/enemy.h"
#include <math.h>
#include <stdlib.h>

#define MAX_ATTACK_TILES 500

void GetAttackRange(GameState state, Player* player, TileMap* map, int rangeTiles[][2], int* count) {
    *count = 0;
    int cx = (int)roundf(player->worldPos.x);
    int cz = (int)roundf(player->worldPos.z);
    int maxRange = 0;
    if (state == MELEE) maxRange = 1;
    else if (state == RANGED) maxRange = 3;
    else if (state == SPELL) maxRange = map->width > map->height ? map->width : map->height; // full map

    for (int dz = -maxRange; dz <= maxRange; dz++) {
        for (int dx = -maxRange; dx <= maxRange; dx++) {
            int dist = abs(dx) + abs(dz);
            if (dist == 0) continue;
            if (state == MELEE && (dx != 0 && dz != 0)) continue;
            if (state == RANGED && dist > 3) continue;
            if (state == MELEE && (dist > 1 || (dx != 0 && dz != 0))) continue;
            int nx = cx + dx, nz = cz + dz;
            if (nx < 0 || nx >= map->width || nz < 0 || nz >= map->height) continue;
            if (*count >= MAX_ATTACK_TILES - 1) break;
            rangeTiles[*count][0] = nx;
            rangeTiles[*count][1] = nz;
            (*count)++;
        }
        if (*count >= MAX_ATTACK_TILES - 1) break;
    }
}

bool IsInAttackRange(GameState state, Player* player, TileMap* map, int tx, int tz) {
    int rangeTiles[MAX_ATTACK_TILES][2];
    int count;
    GetAttackRange(state, player, map, rangeTiles, &count);
    for (int i = 0; i < count; i++) {
        if (rangeTiles[i][0] == tx && rangeTiles[i][1] == tz) return true;
    }
    return false;
}

bool PerformAttack(GameState state, Player* player, TileMap* map, int targetX, int targetZ) {
    // Check if there is an enemy at target tile
    int enemyIdx = GetEnemyAtTile(targetX, targetZ);
    if (enemyIdx == -1) return false;
    // Check if in range
    if (!IsInAttackRange(state, player, map, targetX, targetZ)) return false;
    // Remove enemy
    RemoveEnemy(enemyIdx);
    // Spawn new enemy at random empty tile (avoid player position)
    Vector2 playerPos = { player->worldPos.x, player->worldPos.z };
    SpawnEnemy(map, playerPos);
    return true;
}