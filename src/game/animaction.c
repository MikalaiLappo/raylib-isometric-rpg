#include "game/animation.h"
#include "rendering/isometric.h"
#include <math.h>
#include <stdlib.h>

void InitAnimations(Animation* animations) {
    for (int i = 0; i < MAX_ANIMATIONS; i++) {
        animations[i].active = false;
    }
}

void SpawnAnimation(Animation* animations, AnimationType type, Vector3 worldPos) {
    int idx = -1;
    for (int i = 0; i < MAX_ANIMATIONS; i++) {
        if (!animations[i].active) {
            idx = i;
            break;
        }
    }
    if (idx == -1)
        return;

    animations[idx].type          = type;
    animations[idx].active        = true;
    animations[idx].worldPos      = worldPos;
    animations[idx].timer         = 0.0f;
    animations[idx].duration      = 0.5f;
    animations[idx].currentFrame  = 0;
    animations[idx].totalFrames   = 6;
    animations[idx].frameDuration = animations[idx].duration / animations[idx].totalFrames;
    animations[idx].color         = WHITE;

    switch (type) {
    case ANIM_SWORD:
        animations[idx].color = (Color){255, 255, 255, 255};
        break;
    case ANIM_ARROW:
        animations[idx].color = (Color){160, 120, 80, 255};
        break;
    case ANIM_LIGHTNING:
        animations[idx].color = (Color){255, 255, 100, 255};
        break;
    }
}

void UpdateAnimations(Animation* animations, float dt) {
    for (int i = 0; i < MAX_ANIMATIONS; i++) {
        if (!animations[i].active)
            continue;
        animations[i].timer += dt;
        int frame = (int) (animations[i].timer / animations[i].frameDuration);
        if (frame >= animations[i].totalFrames) {
            animations[i].active = false;
        } else {
            animations[i].currentFrame = frame;
        }
    }
}

static void DrawSwordFrame(Vector2 center, int frame, float scale, Color color) {
    float progress = (float) frame / 6.0f;
    float angle    = progress * 1.5f;
    float radius   = 20 * scale * (0.5f + progress * 0.5f);
    int   segments = 8;
    for (int i = 0; i < segments; i++) {
        float   a1 = -0.5f + (i / (float) segments) * angle;
        float   a2 = -0.5f + ((i + 1) / (float) segments) * angle;
        Vector2 p1 = {center.x + radius * cosf(a1), center.y + radius * sinf(a1)};
        Vector2 p2 = {center.x + radius * cosf(a2), center.y + radius * sinf(a2)};
        Color   c  = color;
        c.a        = (unsigned char) (255 * (1.0f - progress * 0.5f));
        DrawLineEx(p1, p2, 3 * scale, c);
    }
    DrawCircleV(center, 4 * scale, (Color){255, 255, 255, 200});
}

static void DrawArrowFrame(Vector2 center, int frame, float scale, Color color) {
    float progress = (float) frame / 6.0f;
    float offsetY  = -30 * scale * progress;
    float alpha    = (frame < 5) ? 255 : (6 - frame) * 255;
    Color c        = color;
    c.a            = (unsigned char) alpha;

    Vector2 start = {center.x, center.y + offsetY};
    Vector2 end   = {center.x, center.y + offsetY - 20 * scale};
    DrawLineEx(start, end, 2 * scale, c);

    Vector2 p1 = {center.x - 4 * scale, center.y + offsetY - 20 * scale};
    Vector2 p2 = {center.x + 4 * scale, center.y + offsetY - 20 * scale};
    Vector2 p3 = {center.x, center.y + offsetY - 28 * scale};
    DrawTriangle(p1, p2, p3, c);

    // Feathers – using compound literals for the Vector2 arguments
    DrawLineEx((Vector2){center.x - 3 * scale, start.y + 2 * scale},
               (Vector2){center.x - 6 * scale, start.y - 2 * scale}, 1, c);
    DrawLineEx((Vector2){center.x + 3 * scale, start.y + 2 * scale},
               (Vector2){center.x + 6 * scale, start.y - 2 * scale}, 1, c);
}

static void DrawLightningFrame(Vector2 center, int frame, float scale, Color color) {
    float progress = (float) frame / 6.0f;
    float alpha    = (1.0f - progress) * 255;
    int   seed     = frame * 13;
    srand(seed);

    Vector2 points[8];
    points[0] = (Vector2){center.x, center.y - 30 * scale};
    for (int i = 1; i < 7; i++) {
        float x   = center.x + (rand() % 20 - 10) * scale * 0.5f;
        float y   = center.y - 30 * scale + i * 10 * scale;
        points[i] = (Vector2){x, y};
    }
    points[7] = (Vector2){center.x, center.y + 20 * scale};

    Color c = color;
    c.a     = (unsigned char) alpha;
    for (int i = 0; i < 7; i++) {
        DrawLineEx(points[i], points[i + 1], 3 * scale, c);
        if (i % 2 == 0) {
            Vector2 branch =
                (Vector2){points[i].x + (rand() % 10 - 5) * scale, points[i].y + (rand() % 10 - 5) * scale};
            DrawLineEx(points[i], branch, 1 * scale, c);
        }
    }
    DrawCircleV(center, 10 * scale * (1.0f - progress), (Color){255, 255, 100, (unsigned char) (alpha * 0.3f)});
}

void DrawAnimations(const Animation* animations, Vector2 viewOffset, int tileSize) {
    for (int i = 0; i < MAX_ANIMATIONS; i++) {
        if (!animations[i].active)
            continue;
        Vector2 screenPos = WorldToScreen(animations[i].worldPos, viewOffset, tileSize);
        float   progress  = (float) animations[i].currentFrame / animations[i].totalFrames;
        float   scale     = 1.0f + progress * 0.5f;

        switch (animations[i].type) {
        case ANIM_SWORD:
            DrawSwordFrame(screenPos, animations[i].currentFrame, scale, animations[i].color);
            break;
        case ANIM_ARROW:
            DrawArrowFrame(screenPos, animations[i].currentFrame, scale, animations[i].color);
            break;
        case ANIM_LIGHTNING:
            DrawLightningFrame(screenPos, animations[i].currentFrame, scale, animations[i].color);
            break;
        }
    }
}