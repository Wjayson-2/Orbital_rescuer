#include "raylib.h"

#include <algorithm>

#include "AppConstants.hpp"
#include "Input.hpp"
#include "Simulation.hpp"
#include "UI.hpp"

int main() {
    InitWindow(
        kScreenWidth,
        kScreenHeight,
        "Orbital Rescue - C++ Starter Project"
    );
    SetTargetFPS(60);

    Simulation simulation;
    ui::UIState uiState;

    bool paused = false;
    ControlInput controls{};

    while (!WindowShouldClose()) {
        const input::FrameInput frameInput =
            input::handle(simulation, uiState, paused);

        if (frameInput.quitRequested) {
            break;
        }

        controls = frameInput.controls;

        if (simulation.getGameStatus() == GameStatus::Flight && !paused) {
            const double dt = std::min(GetFrameTime(), 0.033f);
            simulation.update(dt, controls);
        }

        BeginDrawing();
        ClearBackground(Color{4, 8, 20, 255});

        ui::draw(simulation, controls, paused, uiState);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
