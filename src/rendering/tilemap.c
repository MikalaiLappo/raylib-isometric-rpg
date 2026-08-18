#include "rendering/tilemap.h"
#include <stdio.h>

TileMap CreateTileMap(int width, int height, int tileSize) {
    TileMap map;
    map.width = width;
    map.height = height;
    map.tileSize = tileSize;
    map.tiles = (Tile*)malloc(sizeof(Tile) * width * height);
    for (int i = 0; i < width * height; i++) {
        map.tiles[i].type = TILE_GRASS;
        map.tiles[i].height = 0;
        map.tiles[i].color = GREEN;
    }
    return map;
}

void UnloadTileMap(TileMap* map) { if (map->tiles) { free(map->tiles); map->tiles = NULL; } }

void GenerateTestMap(TileMap* map) {
    for (int z = 0; z < map->height; z++) {
        for (int x = 0; x < map->width; x++) {
            Tile* tile = GetTile(map, x, z);
            if (x == 2 && z == 2) {
                tile->type = TILE_WATER;
                tile->color = (Color){ 40, 80, 200, 255 };
            } else if (x == 7 && z == 7) {
                tile->type = TILE_STONE;
                tile->color = (Color){ 130, 130, 130, 255 };
            } else if (x == 4 && z == 4) {
                tile->type = TILE_DIRT;
                tile->color = (Color){ 160, 120, 80, 255 };
            } else {
                tile->type = TILE_GRASS;
                if ((x + z) % 2 == 0)
                    tile->color = (Color){ 80, 160, 50, 255 };
                else
                    tile->color = (Color){ 100, 180, 60, 255 };
            }
            tile->height = 0;
        }
    }
}

void DrawTileMap(TileMap* map, Vector2 viewOffset) {
    for (int z = 0; z < map->height; z++) {
        for (int x = 0; x < map->width; x++) {
            Tile* tile = GetTile(map, x, z);
            if (!tile) continue;
            Vector3 pos = { (float)x, (float)tile->height, (float)z };
            Vector2 screenPos = WorldToScreen(pos, viewOffset, map->tileSize);
            DrawIsometricDiamond(screenPos, map->tileSize, tile->color);
        }
    }
}

Tile* GetTile(TileMap* map, int x, int z) {
    if (x < 0 || x >= map->width || z < 0 || z >= map->height) return NULL;
    return &map->tiles[z * map->width + x];
}