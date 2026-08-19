//
// Created by WJays on 2026/8/6.
//

#pragma once


#include "raylib.h"
#include <string>

struct Button {
    Rectangle bounds{};
    std::string text;
    int fontSize{20};

    bool hovered{false};
    bool held{false};
    bool visible{true};

    void update() {
        if (!visible) {
            hovered = false;
            held = false;
            return;
        }

        const Vector2 mouse = GetMousePosition();

        hovered = CheckCollisionPointRec(mouse, bounds);
        held = hovered &&
               IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    }

    bool isClicked() const {
        if (!visible) {
            return false;
        }

        return hovered &&
               IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    }

    void draw() const {
        if (!visible) {
            return;
        }

        Color color = GRAY;

        if (held) {
            color = DARKGRAY;
        } else if (hovered) {
            color = LIGHTGRAY;
        }

        DrawRectangleRec(bounds, color);
        DrawRectangleLinesEx(bounds, 2.0f, WHITE);

        const int textWidth =
            MeasureText(text.c_str(), fontSize);

        const int textX = static_cast<int>(
            bounds.x + (bounds.width - static_cast<double>(textWidth)) / 2.0f
        );

        const int textY = static_cast<int>(
            bounds.y + (bounds.height - static_cast<double>(fontSize)) / 2.0f
        );

        DrawText(
            text.c_str(),
            textX,
            textY,
            fontSize,
            BLACK
        );
    }
};
