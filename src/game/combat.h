#ifndef COMBAT_H
#define COMBAT_H

#include "game/game.h"
#include "game/game_types.h"
#include "player.h"
#include "rendering/tilemap.h"

void GetAttackRange(GameState state, const Player* player, const TileMap* map, int rangeTiles[][2], int* count);
bool IsInAttackRange(GameState state, const Player* player, const TileMap* map, int tx, int tz);
int  PerformAttack(GameState state, const Player* player, const TileMap* map, Game* game, int targetX, int targetZ);

#endif