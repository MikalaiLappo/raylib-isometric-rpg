#include "game/enemy.h"
#include "rendering/isometric.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

Enemy enemies[MAX_ENEMIES];
int   enemyCount = 0;

void InitEnemies(void) {
    enemyCount = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].alive = false;
    }
}

void SpawnEnemy(const TileMap* map, Vector2 avoidPos) {
    if (enemyCount >= MAX_ENEMIES)
        return;
    int  attempts = 0;
    int  x, z;
    bool found = false;
    while (!found && attempts < 1000) {
        x = rand() % map->width;
        z = rand() % map->height;
        if ((int) avoidPos.x == x && (int) avoidPos.y == z)
            continue;
        if (IsTileOccupiedByEnemy(x, z))
            continue;
        found = true;
        attempts++;
    }
    if (!found)
        return;

    int idx = -1;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive) {
            idx = i;
            break;
        }
    }
    if (idx == -1)
        return;

    enemies[idx].pos           = (Vector2){(float) x, (float) z};
    enemies[idx].alive         = true;
    enemies[idx].color         = RED;
    enemies[idx].health        = 10;
    enemies[idx].maxHealth     = 10;
    enemies[idx].hitFlashTimer = 0.0f;
    enemyCount++;
}

static void RemoveEnemy(int index) {
    if (index < 0 || index >= MAX_ENEMIES)
        return;
    if (!enemies[index].alive)
        return;
    enemies[index].alive = false;
    enemyCount--;
}

void TakeDamage(int index, int damage) {
    if (index < 0 || index >= MAX_ENEMIES)
        return;
    if (!enemies[index].alive)
        return;
    enemies[index].health -= damage;
    enemies[index].hitFlashTimer = 0.2f;
    if (enemies[index].health <= 0) {
        RemoveEnemy(index);
    }
}

void DrawEnemies(Vector2 viewOffset, int tileSize) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive)
            continue;
        Vector3 pos3         = {enemies[i].pos.x, 0, enemies[i].pos.y};
        Vector2 screenPos    = WorldToScreen(pos3, viewOffset, tileSize);
        Vector2 shadowScreen = WorldToScreen((Vector3){enemies[i].pos.x, 0, enemies[i].pos.y}, viewOffset, tileSize);
        DrawEllipse(shadowScreen.x, shadowScreen.y, 16, 8, (Color){0, 0, 0, 80});

        Color enemyColor = (enemies[i].hitFlashTimer > 0) ? WHITE : enemies[i].color;
        DrawCircleV(screenPos, 16, enemyColor);
        DrawCircleLines(screenPos.x, screenPos.y, 16, (Color){200, 0, 0, 255});
        DrawCircle(screenPos.x, screenPos.y, 3, (Color){255, 255, 255, 200});
        DrawText("E", screenPos.x - 5, screenPos.y - 8, 16, WHITE);
        int s = 20;
        DrawLine(screenPos.x - s, screenPos.y - s, screenPos.x + s, screenPos.y + s, (Color){255, 0, 0, 180});
        DrawLine(screenPos.x + s, screenPos.y - s, screenPos.x - s, screenPos.y + s, (Color){255, 0, 0, 180});

        float healthPercent = (float) enemies[i].health / enemies[i].maxHealth;
        Color barColor      = (healthPercent > 0.6f) ? GREEN : (healthPercent > 0.3f) ? YELLOW : RED;
        int   barWidth      = 30;
        int   barHeight     = 4;
        int   barX          = screenPos.x - barWidth / 2;
        int   barY          = screenPos.y - 28;
        DrawRectangle(barX, barY, barWidth, barHeight, (Color){50, 50, 50, 200});
        DrawRectangle(barX, barY, (int) (barWidth * healthPercent), barHeight, barColor);
    }
}

bool IsTileOccupiedByEnemy(int tx, int tz) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive)
            continue;
        if ((int) enemies[i].pos.x == tx && (int) enemies[i].pos.y == tz)
            return true;
    }
    return false;
}

int GetEnemyAtTile(int tx, int tz) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive)
            continue;
        if ((int) enemies[i].pos.x == tx && (int) enemies[i].pos.y == tz)
            return i;
    }
    return -1;
}