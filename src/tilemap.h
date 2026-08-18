#ifndef TILEMAP_H
#define TILEMAP_H

#include "raylib.h"
#include "isometric.h"
#include <stdlib.h>

typedef enum { TILE_GRASS, TILE_DIRT, TILE_WATER, TILE_STONE } TileType;

typedef struct {
    TileType type;
    int height;
    Color color;
} Tile;

typedef struct {
    int width, height, tileSize;
    Tile* tiles;
} TileMap;

TileMap CreateTileMap(int width, int height, int tileSize);
void UnloadTileMap(TileMap* map);
void GenerateTestMap(TileMap* map);
void DrawTileMap(TileMap* map, Vector2 viewOffset);
Tile* GetTile(TileMap* map, int x, int z);

#endif