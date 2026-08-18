#include "ui.h"
#include <stdio.h>
#include <string.h>

#define PANEL_HEIGHT 80
#define PANEL_Y (GetScreenHeight() - PANEL_HEIGHT)
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 40
#define BUTTON_SPACING 15
#define BUTTON_Y (PANEL_Y + (PANEL_HEIGHT - BUTTON_HEIGHT) / 2)

void InitUI(UIState* ui) {
    ui->meleeHover  = false;
    ui->rangedHover = false;
    ui->spellHover  = false;
    ui->cancelHover = false;
    strcpy(ui->modeText, "IDLE");
}

void UpdateUI(UIState* ui, Vector2 mouse, GameState state) {
    ui->meleeHover  = false;
    ui->rangedHover = false;
    ui->spellHover  = false;
    ui->cancelHover = false;

    if (mouse.y < PANEL_Y)
        return;

    if (state == IDLE) {
        int       btnStartX = (GetScreenWidth() - (3 * BUTTON_WIDTH + 2 * BUTTON_SPACING)) / 2;
        Rectangle meleeRect = {btnStartX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT};
        if (CheckCollisionPointRec(mouse, meleeRect))
            ui->meleeHover = true;
        Rectangle rangedRect = {btnStartX + BUTTON_WIDTH + BUTTON_SPACING, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT};
        if (CheckCollisionPointRec(mouse, rangedRect))
            ui->rangedHover = true;
        Rectangle spellRect = {btnStartX + 2 * (BUTTON_WIDTH + BUTTON_SPACING), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT};
        if (CheckCollisionPointRec(mouse, spellRect))
            ui->spellHover = true;
    } else {
        int       cancelX    = (GetScreenWidth() - BUTTON_WIDTH) / 2;
        Rectangle cancelRect = {cancelX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT};
        if (CheckCollisionPointRec(mouse, cancelRect))
            ui->cancelHover = true;
    }
}

void DrawUI(UIState* ui, GameState state) {
    DrawRectangle(0, PANEL_Y, GetScreenWidth(), PANEL_HEIGHT, (Color){40, 40, 50, 220});
    DrawLine(0, PANEL_Y, GetScreenWidth(), PANEL_Y, (Color){80, 80, 100, 255});

    if (state == IDLE) {
        int btnStartX = (GetScreenWidth() - (3 * BUTTON_WIDTH + 2 * BUTTON_SPACING)) / 2;
        DrawRectangle(btnStartX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT,
                      ui->meleeHover ? (Color){220, 220, 240, 255} : (Color){70, 70, 90, 255});
        DrawRectangleLines(btnStartX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, (Color){150, 150, 170, 255});
        DrawText("Melee", btnStartX + 25, BUTTON_Y + 10, 18, WHITE);

        int rangedX = btnStartX + BUTTON_WIDTH + BUTTON_SPACING;
        DrawRectangle(rangedX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT,
                      ui->rangedHover ? (Color){220, 220, 240, 255} : (Color){70, 70, 90, 255});
        DrawRectangleLines(rangedX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, (Color){150, 150, 170, 255});
        DrawText("Ranged", rangedX + 20, BUTTON_Y + 10, 18, WHITE);

        int spellX = rangedX + BUTTON_WIDTH + BUTTON_SPACING;
        DrawRectangle(spellX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT,
                      ui->spellHover ? (Color){220, 220, 240, 255} : (Color){70, 70, 90, 255});
        DrawRectangleLines(spellX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, (Color){150, 150, 170, 255});
        DrawText("Spell", spellX + 25, BUTTON_Y + 10, 18, WHITE);
    } else {
        int cancelX = (GetScreenWidth() - BUTTON_WIDTH) / 2;
        DrawRectangle(cancelX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT,
                      ui->cancelHover ? (Color){255, 200, 200, 255} : (Color){200, 70, 70, 255});
        DrawRectangleLines(cancelX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, (Color){255, 150, 150, 255});
        DrawText("X", cancelX + 40, BUTTON_Y + 8, 24, WHITE);
        DrawText(ui->modeText, 10, PANEL_Y + 10, 18, YELLOW);
    }
}

bool IsCancelClicked(UIState* ui, Vector2 mouse, GameState state) {
    if (state == IDLE)
        return false;
    if (mouse.y < PANEL_Y)
        return false;
    int       cancelX    = (GetScreenWidth() - BUTTON_WIDTH) / 2;
    Rectangle cancelRect = {cancelX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT};
    return CheckCollisionPointRec(mouse, cancelRect);
}

bool IsModeButtonClicked(UIState* ui, Vector2 mouse, GameState state, GameState* newMode) {
    if (state != IDLE)
        return false;
    if (mouse.y < PANEL_Y)
        return false;
    int       btnStartX = (GetScreenWidth() - (3 * BUTTON_WIDTH + 2 * BUTTON_SPACING)) / 2;
    Rectangle meleeRect = {btnStartX, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT};
    if (CheckCollisionPointRec(mouse, meleeRect)) {
        *newMode = MELEE;
        return true;
    }
    Rectangle rangedRect = {btnStartX + BUTTON_WIDTH + BUTTON_SPACING, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT};
    if (CheckCollisionPointRec(mouse, rangedRect)) {
        *newMode = RANGED;
        return true;
    }
    Rectangle spellRect = {btnStartX + 2 * (BUTTON_WIDTH + BUTTON_SPACING), BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT};
    if (CheckCollisionPointRec(mouse, spellRect)) {
        *newMode = SPELL;
        return true;
    }
    return false;
}