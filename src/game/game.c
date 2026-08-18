#include "game/game.h"
#include "core/input.h"
#include "game/combat.h"
#include "game/enemy.h"
#include "player.h"
#include "rendering/isometric.h"
#include "rendering/tilemap.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>

void AddFloatingText(Game* game, Vector2 screenPos, const char* text, Color color) {
    if (game->floatingTextCount >= MAX_FLOATING_TEXTS)
        return;
    int idx                            = game->floatingTextCount++;
    game->floatingTexts[idx].screenPos = screenPos;
    strcpy(game->floatingTexts[idx].text, text);
    game->floatingTexts[idx].timer    = 1.0f;
    game->floatingTexts[idx].maxTimer = 1.0f;
    game->floatingTexts[idx].color    = color;
}

void InitGame(Game* game) {
    game->tilemap = CreateTileMap(MAP_SIZE, MAP_SIZE, TILE_SIZE);
    GenerateTestMap(&game->tilemap);

    game->player           = CreatePlayer();
    game->player.worldPos  = (Vector3){MAP_SIZE / 2.0f, 0, MAP_SIZE / 2.0f};
    game->player.targetPos = game->player.worldPos;

    InitEnemies();
    Vector2 playerPos = {game->player.worldPos.x, game->player.worldPos.z};
    for (int i = 0; i < 3; i++) {
        SpawnEnemy(&game->tilemap, playerPos);
    }

    InitUI(&game->ui);

    game->state      = IDLE;
    game->hoverValid = false;
    game->hoverX = game->hoverZ = -1;
    game->hoverEnemyIdx         = -1;
    game->enemyHover            = false;
    game->reachableCount        = 0;
    game->attackCount           = 0;
    game->floatingTextCount     = 0;
}

void UpdateGame(Game* game, float dt) {
    UpdatePlayer(&game->player, &game->tilemap, dt);

    Vector2 playerScreen = WorldToScreen(game->player.worldPos, (Vector2){0, 0}, game->tilemap.tileSize);
    game->viewOffset.x   = SCREEN_WIDTH / 2.0f - playerScreen.x;
    game->viewOffset.y   = SCREEN_HEIGHT / 2.5f - playerScreen.y;

    ProcessInput(game);

    if (game->state == IDLE) {
        GetReachableTiles(&game->player, &game->tilemap, game->reachable, &game->reachableCount);
    } else {
        GetAttackRange(game->state, &game->player, &game->tilemap, game->attackRange, &game->attackCount);
    }

    for (int i = game->floatingTextCount - 1; i >= 0; i--) {
        game->floatingTexts[i].timer -= dt;
        if (game->floatingTexts[i].timer <= 0) {
            game->floatingTexts[i] = game->floatingTexts[game->floatingTextCount - 1];
            game->floatingTextCount--;
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].alive && enemies[i].hitFlashTimer > 0) {
            enemies[i].hitFlashTimer -= dt;
        }
    }
}

void DrawGame(Game* game) {
    BeginDrawing();
    ClearBackground((Color){30, 30, 40, 255});

    DrawTileMap(&game->tilemap, game->viewOffset);

    DrawEnemies(game->viewOffset, game->tilemap.tileSize);

    if (game->state == IDLE) {
        for (int i = 0; i < game->reachableCount; i++) {
            Vector3 pos       = {(float) game->reachable[i][0], 0, (float) game->reachable[i][1]};
            Vector2 screenPos = WorldToScreen(pos, game->viewOffset, game->tilemap.tileSize);
            DrawIsometricDiamond(screenPos, game->tilemap.tileSize, (Color){255, 165, 0, 130});
            int     halfW = game->tilemap.tileSize / 2, halfH = game->tilemap.tileSize / 4;
            Vector2 top     = {screenPos.x, screenPos.y - halfH};
            Vector2 right   = {screenPos.x + halfW, screenPos.y};
            Vector2 bottom  = {screenPos.x, screenPos.y + halfH};
            Vector2 left    = {screenPos.x - halfW, screenPos.y};
            Color   outline = {255, 165, 0, 200};
            DrawLineEx(top, right, 2.0f, outline);
            DrawLineEx(right, bottom, 2.0f, outline);
            DrawLineEx(bottom, left, 2.0f, outline);
            DrawLineEx(left, top, 2.0f, outline);
        }
    } else {
        for (int i = 0; i < game->attackCount; i++) {
            Vector3 pos       = {(float) game->attackRange[i][0], 0, (float) game->attackRange[i][1]};
            Vector2 screenPos = WorldToScreen(pos, game->viewOffset, game->tilemap.tileSize);
            DrawIsometricDiamond(screenPos, game->tilemap.tileSize, (Color){255, 0, 0, 130});
            int     halfW = game->tilemap.tileSize / 2, halfH = game->tilemap.tileSize / 4;
            Vector2 top     = {screenPos.x, screenPos.y - halfH};
            Vector2 right   = {screenPos.x + halfW, screenPos.y};
            Vector2 bottom  = {screenPos.x, screenPos.y + halfH};
            Vector2 left    = {screenPos.x - halfW, screenPos.y};
            Color   outline = {255, 0, 0, 200};
            DrawLineEx(top, right, 2.0f, outline);
            DrawLineEx(right, bottom, 2.0f, outline);
            DrawLineEx(bottom, left, 2.0f, outline);
            DrawLineEx(left, top, 2.0f, outline);
        }
    }

    if (game->hoverValid) {
        Vector3 pos        = {(float) game->hoverX, 0, (float) game->hoverZ};
        Vector2 screenPos  = WorldToScreen(pos, game->viewOffset, game->tilemap.tileSize);
        Color   hoverColor = (game->state == IDLE) ? (Color){255, 200, 50, 180} : (Color){255, 100, 100, 180};
        DrawIsometricDiamond(screenPos, game->tilemap.tileSize, hoverColor);
        DrawCircleLines(screenPos.x, screenPos.y, 24, (Color){255, 200, 0, 255});
        DrawCircleLines(screenPos.x, screenPos.y, 28, (Color){255, 200, 0, 100});
    }

    DrawPlayer(&game->player, game->tilemap.tileSize, game->viewOffset);

    for (int i = 0; i < game->floatingTextCount; i++) {
        float alpha = game->floatingTexts[i].timer / game->floatingTexts[i].maxTimer;
        Color c     = game->floatingTexts[i].color;
        c.a         = (unsigned char) (255 * alpha);
        Vector2 pos = game->floatingTexts[i].screenPos;
        pos.y -= (1.0f - alpha) * 20;
        DrawText(game->floatingTexts[i].text, pos.x - 20, pos.y - 10, 20, c);
    }

    DrawUI(&game->ui, game->state);

    DrawText("ISOMETRIC RPG - Combat", 10, 10, 20, WHITE);
    if (game->state == IDLE)
        DrawText("Click orange tiles to move, or choose attack mode", 10, 40, 16, LIGHTGRAY);
    else
        DrawText("Click on enemy (within red range) to attack, press X to cancel", 10, 40, 16, LIGHTGRAY);
    char pos[100];
    snprintf(pos, sizeof(pos), "Tile: (%d, %d)", (int) roundf(game->player.worldPos.x),
             (int) roundf(game->player.worldPos.z));
    DrawText(pos, 10, 70, 16, LIGHTGRAY);
    if (game->player.isMoving)
        DrawText("Moving...", 10, 95, 16, YELLOW);

    EndDrawing();
}