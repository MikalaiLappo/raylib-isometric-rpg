#ifndef ISOMETRIC_H
#define ISOMETRIC_H

#include "raylib.h"
#include <math.h>

static inline Vector2 WorldToScreen(Vector3 worldPos, Vector2 offset, int tileSize) {
    float   half    = tileSize / 2.0f;
    float   quarter = tileSize / 4.0f;
    Vector2 screenPos;
    screenPos.x = (worldPos.x - worldPos.z) * half + offset.x;
    screenPos.y = (worldPos.x + worldPos.z) * quarter + offset.y;
    screenPos.y -= worldPos.y * half;
    return screenPos;
}

static inline void DrawIsometricDiamond(Vector2 center, int size, Color color) {
    int     halfW  = size / 2;
    int     halfH  = size / 4;
    Vector2 top    = {center.x, center.y - halfH};
    Vector2 right  = {center.x + halfW, center.y};
    Vector2 bottom = {center.x, center.y + halfH};
    Vector2 left   = {center.x - halfW, center.y};
    DrawTriangle(top, right, bottom, color);
    DrawTriangle(top, bottom, left, color);
    Color border = {0, 0, 0, 200};
    DrawLineEx(top, right, 2.0f, border);
    DrawLineEx(right, bottom, 2.0f, border);
    DrawLineEx(bottom, left, 2.0f, border);
    DrawLineEx(left, top, 2.0f, border);
}

#endif