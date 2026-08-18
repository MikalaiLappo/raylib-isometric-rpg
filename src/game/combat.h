#ifndef COMBAT_H
#define COMBAT_H

#include "game/game_types.h"
#include "player.h"
#include "rendering/tilemap.h"

void GetAttackRange(GameState state, Player* player, TileMap* map, int rangeTiles[][2], int* count);
bool IsInAttackRange(GameState state, Player* player, TileMap* map, int tx, int tz);
int  PerformAttack(GameState state, Player* player, TileMap* map, int targetX,
                   int targetZ); // returns damage dealt (0 if no enemy or out of range)

#endif