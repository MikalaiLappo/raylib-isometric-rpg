#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "game/game_types.h" // for GameState

typedef struct {
    bool meleeHover;
    bool rangedHover;
    bool spellHover;
    bool cancelHover;
    char modeText[20];
} UIState;

void InitUI(UIState* ui);
void UpdateUI(UIState* ui, Vector2 mouse, GameState state);
void DrawUI(UIState* ui, GameState state);
bool IsCancelClicked(UIState* ui, Vector2 mouse, GameState state);
bool IsModeButtonClicked(UIState* ui, Vector2 mouse, GameState state, GameState* newMode);

#endif