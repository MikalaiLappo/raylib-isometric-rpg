#ifndef ANIMATION_H
#define ANIMATION_H

#include "game/game_types.h"
#include "raylib.h"

#define MAX_ANIMATIONS 20

typedef enum { ANIM_SWORD, ANIM_ARROW, ANIM_LIGHTNING } AnimationType;

typedef struct {
    AnimationType type;
    bool          active;
    Vector3       worldPos;
    float         timer;
    float         duration;
    int           currentFrame;
    int           totalFrames;
    float         frameDuration;
    Color         color;
} Animation;

void InitAnimations(Animation* animations);
void SpawnAnimation(Animation* animations, AnimationType type, Vector3 worldPos);
void UpdateAnimations(Animation* animations, float dt);
void DrawAnimations(const Animation* animations, Vector2 viewOffset, int tileSize);

#endif