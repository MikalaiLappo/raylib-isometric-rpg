#ifndef UI_H
#define UI_H

#include "game/game_types.h"
#include "raylib.h"

typedef struct {
    bool meleeHover;
    bool rangedHover;
    bool spellHover;
    bool cancelHover;
    char modeText[20];
} UIState;

void InitUI(UIState* ui);
void UpdateUI(UIState* ui, Vector2 mouse, GameState state);
void DrawUI(const UIState* ui, GameState state);
bool IsCancelClicked(const UIState* ui, Vector2 mouse, GameState state);
bool IsModeButtonClicked(const UIState* ui, Vector2 mouse, GameState state, GameState* newMode);

#endif