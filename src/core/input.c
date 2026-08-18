#include "core/input.h"
#include "game/combat.h"
#include "game/enemy.h"
#include "ui.h"
#include "rendering/isometric.h"
#include <stdio.h>
#include <string.h>

void ProcessInput(Game* game) {
    Vector2 mouse = GetMousePosition();
    Vector2 mouseWorld = ScreenToWorld(
        (Vector2){ mouse.x - game->viewOffset.x, mouse.y - game->viewOffset.y },
        game->tilemap.tileSize
    );
    int mouseX = (int)roundf(mouseWorld.x);
    int mouseZ = (int)roundf(mouseWorld.y);

    // Update UI hover states
    UpdateUI(&game->ui, mouse, game->state);
    if (game->state != IDLE) {
        snprintf(game->ui.modeText, sizeof(game->ui.modeText), "Mode: %s",
                 game->state == MELEE ? "Melee" : (game->state == RANGED ? "Ranged" : "Spell"));
    } else {
        strcpy(game->ui.modeText, "IDLE");
    }

    // Enemy hover
    game->enemyHover = false;
    game->hoverEnemyIdx = -1;
    int idx = GetEnemyAtTile(mouseX, mouseZ);
    if (idx != -1 && enemies[idx].alive) {
        game->enemyHover = true;
        game->hoverEnemyIdx = idx;
    }

    // Handle clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        GameState newMode;
        if (IsModeButtonClicked(&game->ui, mouse, game->state, &newMode)) {
            game->state = newMode;
            printf("Switched to %s mode\n", game->state == MELEE ? "Melee" : (game->state == RANGED ? "Ranged" : "Spell"));
        } else if (IsCancelClicked(&game->ui, mouse, game->state)) {
            game->state = IDLE;
            printf("Canceled attack mode\n");
        } else if (game->state == IDLE) {
            if (mouseX >= 0 && mouseX < game->tilemap.width && mouseZ >= 0 && mouseZ < game->tilemap.height) {
                if (IsTileReachable(&game->player, &game->tilemap, mouseX, mouseZ)) {
                    MovePlayerToTile(&game->player, mouseX, mouseZ);
                }
            }
        } else {
            // Attack mode
            if (game->enemyHover && IsInAttackRange(game->state, &game->player, &game->tilemap, mouseX, mouseZ)) {
                bool success = PerformAttack(game->state, &game->player, &game->tilemap, mouseX, mouseZ);
                if (success) {
                    printf("Enemy destroyed! New enemy spawned.\n");
                }
            }
        }
    }

    // Keyboard movement (arrow keys / WASD)
    if (game->state == IDLE && !game->player.isMoving) {
        int cx = (int)roundf(game->player.worldPos.x);
        int cz = (int)roundf(game->player.worldPos.z);
        int nx = cx, nz = cz;
        bool moved = false;
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) { nz--; moved = true; }
        else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) { nz++; moved = true; }
        else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) { nx--; moved = true; }
        else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) { nx++; moved = true; }
        if (moved && nx >= 0 && nx < game->tilemap.width && nz >= 0 && nz < game->tilemap.height) {
            if (!IsTileOccupied(nx, nz)) MovePlayerToTile(&game->player, nx, nz);
        }
    }

    // Update hover highlight for tiles
    game->hoverValid = false;
    if (game->state == IDLE) {
        if (mouseX >= 0 && mouseX < game->tilemap.width && mouseZ >= 0 && mouseZ < game->tilemap.height) {
            if (IsTileReachable(&game->player, &game->tilemap, mouseX, mouseZ)) {
                game->hoverValid = true;
                game->hoverX = mouseX;
                game->hoverZ = mouseZ;
            }
        }
    } else {
        if (mouseX >= 0 && mouseX < game->tilemap.width && mouseZ >= 0 && mouseZ < game->tilemap.height) {
            if (IsInAttackRange(game->state, &game->player, &game->tilemap, mouseX, mouseZ)) {
                game->hoverValid = true;
                game->hoverX = mouseX;
                game->hoverZ = mouseZ;
            }
        }
    }
}