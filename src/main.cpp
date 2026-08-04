#include "raylib.h"
#include "Simulation.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "MissionConfig.h"




namespace {
constexpr int kScreenWidth = 1280;
constexpr int kScreenHeight = 760;
constexpr double kWorldScale = 1.0;

Vector2 worldToScreen(const Vec2& world) {
    return {
        static_cast<float>(kScreenWidth * 0.5 + world.x * kWorldScale),
        static_cast<float>(kScreenHeight * 0.5 - world.y * kWorldScale)
    };
}

std::string fixed(double value, int decimals = 1) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(decimals) << value;
    return out.str();
}

void drawStars() {
    // Deterministic pseudo-starfield: no asset files required.
    for (int i = 0; i < 160; ++i) {
        const int x = (i * 97 + 41) % kScreenWidth;
        const int y = (i * 53 + 17) % kScreenHeight;
        const float radius = (i % 7 == 0) ? 1.6f : 1.0f;
        DrawCircle(x, y, radius, Fade(RAYWHITE, 0.55f));
    }
}

void drawStation(const Vec2& position) {
    const Vector2 p = worldToScreen(position);

    DrawRectangle(static_cast<int>(p.x - 46), static_cast<int>(p.y - 24), 92, 48, DARKGRAY);
    DrawRectangle(static_cast<int>(p.x - 8), static_cast<int>(p.y - 44), 16, 88, GRAY);
    DrawRectangle(static_cast<int>(p.x - 112), static_cast<int>(p.y - 12), 62, 24, BLUE);
    DrawRectangle(static_cast<int>(p.x + 50), static_cast<int>(p.y - 12), 62, 24, BLUE);

    DrawCircleLines(static_cast<int>(p.x - 48), static_cast<int>(p.y), 24, SKYBLUE);
    DrawCircleLines(static_cast<int>(p.x - 48), static_cast<int>(p.y), 37, Fade(SKYBLUE, 0.45f));

    DrawText("ARES RELAY", static_cast<int>(p.x - 34), static_cast<int>(p.y + 52), 16, LIGHTGRAY);
}

void drawTrail() {
    if (trail_.size() > 2) {
        for (int i = 0; i+1 < trail_.size(); ++i) {
            //Vector2 start{static_cast<float>(trail_[i].x), static_cast<float>(trail_[i].y)};
            //Vector2 end{static_cast<float>(trail_[i+1].x), static_cast<float>(trail_[i+1].y)};
            float alpha = 1.0f * i / trail_.size();

            DrawLineV(worldToScreen(trail_[i]), worldToScreen(trail_[i+1]), Fade(SKYBLUE, alpha));
        }
    }else {
        return;
    }
}

void drawSpacecraft(const Spacecraft& craft, const ControlInput& input) {
    const Vector2 p = worldToScreen(craft.position());

    // Convert world rotation to screen rotation.
    // Screen Y increases downward, so the angle must be negated once.
    const float screenAngle =
        static_cast<float>(-craft.angleRadians());

    const Vector2 nose{
        p.x + 28.0f * std::cos(screenAngle),
        p.y + 28.0f * std::sin(screenAngle)
    };

    const Vector2 left{
        p.x + 18.0f * std::cos(screenAngle + 145.0f * DEG2RAD),
        p.y + 18.0f * std::sin(screenAngle + 145.0f * DEG2RAD)
    };

    const Vector2 right{
        p.x + 18.0f * std::cos(screenAngle - 145.0f * DEG2RAD),
        p.y + 18.0f * std::sin(screenAngle - 145.0f * DEG2RAD)
    };

    DrawTriangle(nose, left, right, RAYWHITE);
    DrawTriangleLines(nose, left, right, SKYBLUE);

    if (input.mainThrottle > 0.01 && craft.hasFuel()) {
        const float flameLength =
            static_cast<float>(25.0 + 25.0 * input.mainThrottle);

        const Vector2 flame{
            p.x - flameLength * std::cos(screenAngle),
            p.y - flameLength * std::sin(screenAngle)
        };

        DrawTriangle(left, right, flame, ORANGE);


    }
}

void drawTelemetry(const Simulation& sim) {
    const Spacecraft& craft = sim.craft();
    const Vec2 relative = sim.stationPosition() - craft.position();

    DrawRectangle(18, 18, 330, 248, Fade(BLACK, 0.72f));
    DrawRectangleLines(18, 18, 330, 248, Fade(SKYBLUE, 0.8f));

    DrawText("FLIGHT COMPUTER", 34, 32, 24, SKYBLUE);
    DrawText(("MISSION TIME  " + fixed(sim.missionTime(), 1) + " s").c_str(), 34, 70, 20, RAYWHITE);
    DrawText(("DISTANCE      " + fixed(relative.magnitude(), 1) + " m").c_str(), 34, 98, 20, RAYWHITE);
    DrawText(("SPEED         " + fixed(craft.velocity().magnitude(), 2) + " m/s").c_str(), 34, 126, 20, RAYWHITE);
    DrawText(("ANGLE         " + fixed(craft.angleRadians() * RAD2DEG, 1) + " deg").c_str(), 34, 154, 20, RAYWHITE);
    DrawText(("FUEL          " + fixed(craft.fuel(), 1) + " kg").c_str(), 34, 182, 20, YELLOW);
    DrawText(("HULL          " + fixed(craft.hullIntegrity(), 0) + " %").c_str(), 34, 210, 20,
             craft.hullIntegrity() > 35.0 ? GREEN : RED);

    const DockingResult docking = sim.evaluateDocking();
    DrawText(docking.speedSafe ? "SPEED SAFE" : "SPEED HIGH", 900, 28, 20,
             docking.speedSafe ? GREEN : RED);
    DrawText(docking.angleSafe ? "ALIGNMENT SAFE" : "ALIGNMENT BAD", 900, 56, 20,
             docking.angleSafe ? GREEN : RED);
}

void drawHelp() {
    std::string text;
    DrawRectangle(18, kScreenHeight - 106, 600, 84, Fade(BLACK, 0.70f));
    DrawText("W: main engine     A,D: side thrusters", 32, kScreenHeight - 92, 18, LIGHTGRAY);
    DrawText("Q / E: rotate    R: reset    P: pause", 32, kScreenHeight - 66, 18, LIGHTGRAY);
    text = "Goal: dock below " + fixed(config_global.dockingLimits.safeSpeed,2) + " m/s and within " + fixed(config_global.dockingLimits.safeAngleRadians,2) + " rad.";
    DrawText(text.c_str(), 32, kScreenHeight - 40, 18, SKYBLUE);
}

ControlInput readControls() {
    ControlInput input;

    if (IsKeyDown(KEY_W)) {
        input.mainThrottle = 1.0;
    }
    if (IsKeyDown(KEY_D)) {
        input.lateralThrottle -= 1.0;
    }
    if (IsKeyDown(KEY_A)) {
        input.lateralThrottle += 1.0;
    }
    if (IsKeyDown(KEY_E)) {
        input.rotationInput -= 1.0;
    }
    if (IsKeyDown(KEY_Q)) {
        input.rotationInput += 1.0;
    }

    return input;
}
}

int main() {
    loadMissionConfig("./missions/mission_01.txt");
    InitWindow(kScreenWidth, kScreenHeight, "Orbital Rescue - C++ Starter Project");
    SetTargetFPS(60);

    Simulation simulation;
    bool paused = false;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ENTER)) {
            simulation.start();
        }

        if (IsKeyPressed(KEY_R)) {
            simulation.reset();
            paused = false;
        }

        if (IsKeyPressed(KEY_P)) {
            paused = !paused;
        }

        const ControlInput input = readControls();

        if (!paused) {
            const double dt = std::min(GetFrameTime(), 0.033f);
            simulation.update(dt, input);
        }

        BeginDrawing();
        ClearBackground(Color{4, 8, 20, 255});

        drawStars();
        drawStation(simulation.stationPosition());
        drawSpacecraft(simulation.craft(), input);
        drawTelemetry(simulation);
        drawHelp();
        drawTrail();

        const std::string status = simulation.statusMessage();
        const int textWidth = MeasureText(status.c_str(), 30);
        DrawText(status.c_str(), (kScreenWidth - textWidth) / 2, 20, 30, GOLD);

        if (paused) {
            DrawText("PAUSED", kScreenWidth / 2 - 65, kScreenHeight / 2 - 20, 36, YELLOW);
        }

        if (simulation.state() == MissionState::Docked) {
            const std::string score = "SCORE: " + std::to_string(simulation.score());
            DrawRectangle(kScreenWidth / 2 - 180, kScreenHeight / 2 - 70, 360, 140, Fade(BLACK, 0.86f));
            DrawText("RESCUE SUCCESSFUL", kScreenWidth / 2 - 145, kScreenHeight / 2 - 45, 26, GREEN);
            DrawText(score.c_str(), kScreenWidth / 2 - 70, kScreenHeight / 2, 24, RAYWHITE);
            DrawText("Press R for another attempt", kScreenWidth / 2 - 135,
                     kScreenHeight / 2 + 36, 18, LIGHTGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
