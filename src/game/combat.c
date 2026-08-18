#include "game/combat.h"
#include "game/animation.h"
#include "game/enemy.h"
#include <math.h>
#include <stdlib.h>

#define MAX_ATTACK_TILES 500

void GetAttackRange(GameState state, const Player* player, const TileMap* map, int rangeTiles[][2], int* count) {
    *count       = 0;
    int cx       = (int) roundf(player->worldPos.x);
    int cz       = (int) roundf(player->worldPos.z);
    int maxRange = 0;
    if (state == MELEE)
        maxRange = 1;
    else if (state == RANGED)
        maxRange = 3;
    else if (state == SPELL)
        maxRange = map->width > map->height ? map->width : map->height;

    for (int dz = -maxRange; dz <= maxRange; dz++) {
        for (int dx = -maxRange; dx <= maxRange; dx++) {
            int dist = abs(dx) + abs(dz);
            if (dist == 0)
                continue;
            if (state == MELEE && (dx != 0 && dz != 0))
                continue;
            if (state == RANGED && dist > 3)
                continue;
            if (state == MELEE && (dist > 1 || (dx != 0 && dz != 0)))
                continue;
            int nx = cx + dx, nz = cz + dz;
            if (nx < 0 || nx >= map->width || nz < 0 || nz >= map->height)
                continue;
            if (*count >= MAX_ATTACK_TILES - 1)
                break;
            rangeTiles[*count][0] = nx;
            rangeTiles[*count][1] = nz;
            (*count)++;
        }
        if (*count >= MAX_ATTACK_TILES - 1)
            break;
    }
}

bool IsInAttackRange(GameState state, const Player* player, const TileMap* map, int tx, int tz) {
    int rangeTiles[MAX_ATTACK_TILES][2];
    int count;
    GetAttackRange(state, player, map, rangeTiles, &count);
    for (int i = 0; i < count; i++) {
        if (rangeTiles[i][0] == tx && rangeTiles[i][1] == tz)
            return true;
    }
    return false;
}

int PerformAttack(GameState state, const Player* player, const TileMap* map, Game* game, int targetX, int targetZ) {
    int enemyIdx = GetEnemyAtTile(targetX, targetZ);
    if (enemyIdx == -1)
        return 0;
    if (!IsInAttackRange(state, player, map, targetX, targetZ))
        return 0;

    int           damage   = 0;
    AnimationType animType = ANIM_SWORD;
    switch (state) {
    case MELEE:
        damage   = 5;
        animType = ANIM_SWORD;
        break;
    case RANGED:
        damage   = 3;
        animType = ANIM_ARROW;
        break;
    case SPELL:
        damage   = 2;
        animType = ANIM_LIGHTNING;
        break;
    default:
        damage = 0;
        break;
    }
    if (damage == 0)
        return 0;

    TakeDamage(enemyIdx, damage);

    Vector3 enemyPos = {(float) targetX, 0, (float) targetZ};
    SpawnAnimation(game->animations, animType, enemyPos);

    return damage;
}