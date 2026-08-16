#include "raylib.h"
#include "Simulation.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "MissionConfig.hpp"
#include "Buttons.hpp"
#include "AP.hpp"





namespace {
constexpr int kScreenWidth = 1280;
constexpr int kScreenHeight = 760;
constexpr double kWorldScale = 1.0;

    //_____________________________________________
    Button timescalex1{
        .bounds = Rectangle{1190, kScreenHeight - 92, 70, 70},
        .text ="x1",
        .fontSize = 56,
        .visible = true
    };
    Button timescalex2{
        .bounds = Rectangle{1190, kScreenHeight - 92, 70, 70},
        .text ="x2",
        .fontSize = 56,
        .visible = false
    };
    Button timescalex5{
        .bounds = Rectangle{1190, kScreenHeight - 92, 70, 70},
        .text ="x5",
        .fontSize = 56,
        .visible = false
    };
    //Timescale button
    //_____________________________________________
    Button startButton{
        .bounds = Rectangle{480.0f, 350.0f, 320.0f, 64.0f},
        .text = "START",
        .fontSize = 30,
        .visible = true
    };

    Button quitButton{
        .bounds = Rectangle{480.0f, 435.0f, 320.0f, 64.0f},
        .text = "QUIT",
        .fontSize = 30,
        .visible = true
    };

    //Menu button
    //_____________________________________________

    Button mission1Button{
        .bounds = Rectangle{450.0f, 245.0f, 380.0f, 64.0f},
        .text = "MISSION 01 - ARES RELAY",
        .fontSize = 23,
        .visible = true
    };

    Button mission2Button{
        .bounds = Rectangle{450.0f, 329.0f, 380.0f, 64.0f},
        .text = "MISSION 02 - LUNAR ORBIT",
        .fontSize = 23,
        .visible = true
    };

    Button mission3Button{
        .bounds = Rectangle{450.0f, 413.0f, 380.0f, 64.0f},
        .text = "MISSION 03 - DEEP SPACE",
        .fontSize = 23,
        .visible = true
    };

    Button missionBackButton{
        .bounds = Rectangle{520.0f, 525.0f, 240.0f, 56.0f},
        .text = "BACK",
        .fontSize = 26,
        .visible = true
    };
    //Mission button
    //_____________________________________________

    Button statButton{
        .bounds = Rectangle{480.0f, 390.0f, 320.0f, 60.0f},
        .text = "STATS",
        .fontSize = 28,
        .visible = true
    };

    Button retryButton{
        .bounds = Rectangle{480.0f, 470.0f, 320.0f, 60.0f},
        .text = "RETRY",
        .fontSize = 25,
        .visible = true
    };

    Button selectMissionButton{
        .bounds = Rectangle{480.0f, 550.0f, 320.0f, 60.0f},
        .text = "SELECT MISSION",
        .fontSize = 26,
        .visible = true
    };
    // Results menu buttons
    //_____________________________________________

    Button retryButton_2{
        .bounds = Rectangle{300.0f, 640.0f, 190.0f, 52.0f},
        .text = "RETRY",
        .fontSize = 24,
        .visible = true
    };

    Button trailButton{
        .bounds = Rectangle{520.0f, 640.0f, 240.0f, 52.0f},
        .text = "SHOW TRAIL",
        .fontSize = 22,
        .visible = true
    };

    Button selectMissionButton_2{
        .bounds = Rectangle{790.0f, 640.0f, 190.0f, 52.0f},
        .text = "MENU",
        .fontSize = 22,
        .visible = true
    };
    //Draw stat buttons
    //_____________________________________________
    Button returnButton{
        .bounds = Rectangle{520.0f, 640.0f, 240.0f, 52.0f},
        .text = "RETURN",
        .fontSize = 22,
        .visible = true
    };
    //Back
    //_____________________________________________
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

void drawStation(const Vec2& position, const Vec2& port_) {
    const Vector2 p = worldToScreen(position);
    const Vector2 port = worldToScreen(port_);

    DrawRectangle(static_cast<int>(p.x - 46), static_cast<int>(p.y - 24), 92, 48, DARKGRAY);
    DrawRectangle(static_cast<int>(p.x - 8), static_cast<int>(p.y - 44), 16, 88, GRAY);
    DrawRectangle(static_cast<int>(p.x - 112), static_cast<int>(p.y - 12), 62, 24, BLUE);
    DrawRectangle(static_cast<int>(p.x + 50), static_cast<int>(p.y - 12), 62, 24, BLUE);

    DrawCircleLines(static_cast<int>(port.x), static_cast<int>(port.y), static_cast<float>(config_global.dockingLimits.captureDistance), SKYBLUE);
    DrawCircleLines(static_cast<int>(port.x), static_cast<int>(port.y), static_cast<float>(config_global.dockingLimits.captureDistance+13), Fade(SKYBLUE, 0.45f));

    DrawText("ARES RELAY", static_cast<int>(p.x - 34), static_cast<int>(p.y + 52), 16, LIGHTGRAY);
}

void drawTrail(const Simulation& simulation) {
    std::size_t count = simulation.getTrailCount();

    if (count < 2) {
        return;
    }

    for (std::size_t i = 0; i + 1 < count; ++i) {
        float alpha =
            static_cast<float>(i + 1) /
            static_cast<float>(count);

        DrawLineV(
            worldToScreen(simulation.getTrailPoint(i)),
            worldToScreen(simulation.getTrailPoint(i + 1)),
            Fade(SKYBLUE, alpha)
        );
    }
}

void drawSpacecraft(const Spacecraft& craft, const ControlInput& input) {
    const Vector2 p = worldToScreen(craft.position());
    DrawCircleV(p, 3.0f, RED);
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

void drawPlanet() {
    const Vector2 p = worldToScreen(config_global.planetspec.position);
    DrawCircleV(p, static_cast<float>(config_global.planetspec.radius), GRAY);
}

void drawTelemetry(const Simulation& sim) {
    const Spacecraft& craft = sim.craft();
    const Vec2 relative = sim.stationPosition() - craft.position();

    DrawRectangle(18, 18, 330, 276, Fade(BLACK, 0.72f));
    DrawRectangleLines(18, 18, 330, 276, Fade(SKYBLUE, 0.8f));

    DrawText("FLIGHT COMPUTER", 34, 32, 24, SKYBLUE);


    DrawText(("MISSION TIME  "), 34, 70, 20, (sim.missionTime()/config_global.timeLimit < 0.75)? GREEN : RED);
    DrawText(("DISTANCE" ), 34, 98, 20, RAYWHITE);
    DrawText(("SPEED" ), 34, 126, 20, RAYWHITE);
    DrawText(("ANGLE" ), 34, 154, 20, RAYWHITE);
    DrawText(("FUEL" ), 34, 182, 20, YELLOW);
    DrawText(("HULL" ), 34, 210, 20, craft.hullIntegrity() > 35.0 ? GREEN : RED);
    DrawText((fixed(sim.missionTime(), 1) + "/" + fixed(config_global.timeLimit) + " s").c_str(), 200, 70, 20, (sim.missionTime()/config_global.timeLimit < 0.75)? GREEN : RED);
    DrawText((fixed(relative.magnitude(), 1) + " m").c_str(), 200, 98, 20, RAYWHITE);
    DrawText((fixed(craft.velocity().magnitude(), 2) + " m/s").c_str(), 200, 126, 20, RAYWHITE);
    DrawText((fixed(craft.angleRadians() * RAD2DEG, 1) + " deg").c_str(), 200, 154, 20, RAYWHITE);
    DrawText((fixed(craft.fuel(), 1) + " kg").c_str(), 200, 182, 20, YELLOW);
    DrawText((fixed(craft.hullIntegrity(), 0) + " %").c_str(), 200, 210, 20, craft.hullIntegrity() > 35.0 ? GREEN : RED);

    const DockingResult docking = sim.evaluateDocking();
    DrawText(docking.speedSafe ? "SPEED SAFE" : "SPEED HIGH", 900, 28, 20,
             docking.speedSafe ? GREEN : RED);
    DrawText(docking.angleSafe ? "ALIGNMENT SAFE" : "ALIGNMENT BAD", 900, 56, 20,
             docking.angleSafe ? GREEN : RED);

    DrawText(
    "AUTOPILOT",
        34,
        238,
        20,
        sim.APisENGAGED() ? GREEN : LIGHTGRAY
    );


    DrawText(
    sim.APisENGAGED() ? "ENGAGED" : "OFF",
        200,
        238,
        20,
        sim.APisENGAGED() ? GREEN : LIGHTGRAY
    );

    std::string text;



    if (sim.getAPstatus() == AP_status::Abort) {
        text = "Abort";
    }else if (sim.getAPstatus() == AP_status::Align) {
        text = "Align";
    }else if (sim.getAPstatus() == AP_status::Approach) {
        text = "Approach";
    }else if (sim.getAPstatus() == AP_status::Brake) {
        text = "Brake";
    }else if (sim.getAPstatus() == AP_status::Docked) {
        text = "Docked";
    }else if (sim.getAPstatus() == AP_status::Final_approach) {
        text = "Final Approach";
    }
    if (sim.APisENGAGED()) {
        DrawText(
       "STAGE",
       34,
       266,
       20,
       SKYBLUE
   );

        DrawText(
        text.c_str(),
        200,
        266,
        20,
        text == "Abort" ? RED : SKYBLUE
    );
    }

}

void drawHelp() {
    std::string text;
    DrawRectangle(18, kScreenHeight - 106, 600, 84, Fade(BLACK, 0.70f));
    DrawText("W: main engine     A,D: side thrusters    <: Speed Dn     Z: Autopilot", 32, kScreenHeight - 92, 18, LIGHTGRAY);
    DrawText("Q / E: rotate    R: reset    P: pause     >: Speed Up", 32, kScreenHeight - 66, 18, LIGHTGRAY);
    text = "Goal: dock below " + fixed(config_global.dockingLimits.safeSpeed,2) + " m/s and within " + fixed(config_global.dockingLimits.safeAngleRadians,2) + " rad.";
    DrawText(text.c_str(), 32, kScreenHeight - 40, 18, SKYBLUE);
    DrawText("M: MENU", 445, kScreenHeight - 40, 18, LIGHTGRAY);

}

// int AP_flash_flag = 0; //to record the time the sign AP has flashed

// void drawAP(const Simulation& simulation) {
//     if (simulation.APisENGAGED()) {
//         AP_flash_flag += 1;
//         if ((AP_flash_flag % 60) < 30) { //turn on/off every 30 frames
//             DrawText("AP", kScreenWidth / 2 + 30, kScreenHeight - 92,  28, YELLOW);
//         }
//     }
//     if (AP_flash_flag == 60) {
//         AP_flash_flag = 0; //keep it bounded
//     }
// }

void drawButtons() {

        timescalex1.draw();
        timescalex2.draw();
        timescalex5.draw();

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


void drawTimescale(TimeScale timescale) {
    std::string text;
    if (timescale == TimeScale::x1) {
        text = "x1";
        timescalex1.visible = true;
        timescalex2.visible = false;
        timescalex5.visible = false;
    }
    else if (timescale == TimeScale::x2) {
        text = "x2";
        timescalex2.visible = true;
        timescalex5.visible = false;
        timescalex1.visible = false;
    }
    else if (timescale == TimeScale::x5) {
        text = "x5";
        timescalex5.visible = true;
        timescalex2.visible = false;
        timescalex1.visible = false;
    }
    //DrawText(text.c_str(), 1190, kScreenHeight - 92, 56, LIGHTGRAY);
}

void drawCenteredText(
        const std::string& text,
        int y,
        int fontSize,
        Color color
    ) {
    const int width = MeasureText(text.c_str(), fontSize);

    DrawText(
        text.c_str(),
        (kScreenWidth - width) / 2,
        y,
        fontSize,
        color
    );
}

void drawMainMenu() {
    drawStars();

    const Rectangle panel{
        360.0f,
        110.0f,
        560.0f,
        520.0f
    };

    DrawRectangleRec(panel, Fade(BLACK, 0.78f));
    DrawRectangleLinesEx(panel, 3.0f, SKYBLUE);

    drawCenteredText(
        "ORBITAL RESCUE",
        170,
        52,
        SKYBLUE
    );

    drawCenteredText(
        "SPACECRAFT DOCKING SIMULATOR",
        240,
        22,
        LIGHTGRAY
    );

    DrawLine(
        440,
        300,
        840,
        300,
        Fade(SKYBLUE, 0.65f)
    );

    startButton.draw();
    quitButton.draw();

    drawCenteredText(
        "WASD / QE flight controls",
        555,
        18,
        GRAY
    );

    drawCenteredText(
        "C++ / raylib",
        585,
        16,
        DARKGRAY
    );
}

void drawFPS() {
    DrawFPS(1190, 28);
}
void drawMissionSelectMenu() {
    drawStars();

    const Rectangle panel{
        350.0f,
        85.0f,
        580.0f,
        590.0f
    };

    DrawRectangleRec(panel, Fade(BLACK, 0.80f));
    DrawRectangleLinesEx(panel, 3.0f, SKYBLUE);

    drawCenteredText(
        "SELECT MISSION",
        125,
        42,
        SKYBLUE
    );

    drawCenteredText(
        "Choose an orbital rescue assignment",
        185,
        20,
        LIGHTGRAY
    );

    mission1Button.draw();
    mission2Button.draw();
    mission3Button.draw();
    missionBackButton.draw();
}
void drawResultsMenu(const Simulation& simulation) {
    drawStars();

    const bool success =
        simulation.state() == MissionState::Docked;

    const Rectangle panel{
        360.0f,
        100.0f,
        560.0f,
        570.0f
    };

    DrawRectangleRec(panel, Fade(BLACK, 0.86f));

    DrawRectangleLinesEx(
        panel,
        3.0f,
        success ? GREEN : RED
    );

    drawCenteredText(
        success ? "MISSION SUCCESS" : "MISSION FAILED",
        155,
        44,
        success ? GREEN : RED
    );

    if (success) {
        drawCenteredText(
            "Docking procedure completed",
            225,
            22,
            LIGHTGRAY
        );

        drawCenteredText(
            "SCORE: " + std::to_string(simulation.score()),
            295,
            34,
            GOLD
        );
    } else {
        drawCenteredText(
            simulation.statusMessage(),
            235,
            22,
            LIGHTGRAY
        );

        drawCenteredText(
            "Review your approach and try again",
            295,
            20,
            GRAY
        );
    }

    retryButton.draw();
    statButton.draw();
    selectMissionButton.draw();
}
    void drawStatCard(
    Rectangle bounds,
    const std::string& title,
    const std::string& value,
    const std::string& detail,
    Color valueColor
) {
    DrawRectangleRec(
        bounds,
        Fade(Color{12, 20, 34, 255}, 0.90f)
    );

    DrawRectangleLinesEx(
        bounds,
        2.0f,
        Fade(SKYBLUE, 0.55f)
    );

    DrawText(
        title.c_str(),
        static_cast<int>(bounds.x + 18.0f),
        static_cast<int>(bounds.y + 13.0f),
        17,
        LIGHTGRAY
    );

    const int valueFontSize = 30;
    const int valueWidth =
        MeasureText(value.c_str(), valueFontSize);

    DrawText(
        value.c_str(),
        static_cast<int>(
            bounds.x +
            (bounds.width - static_cast<float>(valueWidth)) / 2.0f
        ),
        static_cast<int>(bounds.y + 43.0f),
        valueFontSize,
        valueColor
    );

    if (!detail.empty()) {
        const int detailFontSize = 15;
        const int detailWidth =
            MeasureText(detail.c_str(), detailFontSize);

        DrawText(
            detail.c_str(),
            static_cast<int>(
                bounds.x +
                (bounds.width - static_cast<float>(detailWidth)) / 2.0f
            ),
            static_cast<int>(bounds.y + 86.0f),
            detailFontSize,
            GRAY
        );
    }
}


// ============================================================
// Hull integrity progress bar
// ============================================================

void drawHullBar(
    Rectangle bounds,
    double hullIntegrity,
    Color barColor
) {
    const float percentage = static_cast<float>(
        std::clamp(hullIntegrity, 0.0, 100.0) / 100.0
    );

    DrawRectangleRec(
        bounds,
        Color{35, 40, 48, 255}
    );

    Rectangle filled{
        bounds.x,
        bounds.y,
        bounds.width * percentage,
        bounds.height
    };

    DrawRectangleRec(filled, barColor);
    DrawRectangleLinesEx(bounds, 1.0f, LIGHTGRAY);
}


// ============================================================
// Complete results / statistics page
// ============================================================

void drawStatsMenu(
    bool missionSuccessful,
    const std::string& missionName,
    double startFuel,
    double finalFuel,
    double missionTime,
    double maxSpeed,
    double impactSpeed,
    double safeImpactSpeed,
    double hullIntegrity,
    int score
) {
    drawStars();

    const Color resultColor =
        missionSuccessful ? GREEN : RED;

    // Main results panel
    const Rectangle panel{
        240.0f,
        45.0f,
        800.0f,
        670.0f
    };

    DrawRectangleRec(
        panel,
        Fade(Color{3, 8, 18, 255}, 0.92f)
    );

    DrawRectangleLinesEx(
        panel,
        3.0f,
        resultColor
    );

    // Decorative upper corners
    DrawLine(240, 90, 280, 90, resultColor);
    DrawLine(240, 90, 240, 130, resultColor);

    DrawLine(1040, 90, 1000, 90, resultColor);
    DrawLine(1040, 90, 1040, 130, resultColor);

    // Main heading
    drawCenteredText(
        missionSuccessful
            ? "MISSION SUCCESS"
            : "MISSION FAILED",
        72,
        44,
        resultColor
    );

    drawCenteredText(
        missionName,
        125,
        21,
        LIGHTGRAY
    );

    DrawLine(
        310,
        158,
        970,
        158,
        Fade(SKYBLUE, 0.60f)
    );

    // --------------------------------------------------------
    // Card layout
    // --------------------------------------------------------

    constexpr float cardWidth = 330.0f;
    constexpr float cardHeight = 112.0f;

    constexpr float leftX = 285.0f;
    constexpr float rightX = 665.0f;

    constexpr float row1Y = 180.0f;
    constexpr float row2Y = 315.0f;
    constexpr float row3Y = 450.0f;

    const double fuelUsed =
        std::max(0.0, startFuel - finalFuel);

    // Fuel card
    drawStatCard(
        Rectangle{
            leftX,
            row1Y,
            cardWidth,
            cardHeight
        },
        "FUEL CONSUMED",
        fixed(fuelUsed, 1) + " kg",
        fixed(startFuel, 1) +
            "  ->  " +
            fixed(finalFuel, 1) +
            " kg",
        YELLOW
    );

    // Mission time card
    drawStatCard(
        Rectangle{
            rightX,
            row1Y,
            cardWidth,
            cardHeight
        },
        "MISSION TIME",
        fixed(missionTime, 2) + " s",
        "TOTAL FLIGHT DURATION",
        SKYBLUE
    );

    // Maximum speed card
    drawStatCard(
        Rectangle{
            leftX,
            row2Y,
            cardWidth,
            cardHeight
        },
        "MAX SPEED",
        fixed(maxSpeed, 2) + " m/s",
        "MISSION PEAK",
        ORANGE
    );

    // Impact speed card
    const bool impactSpeedSafe =
        impactSpeed <= safeImpactSpeed;

    drawStatCard(
        Rectangle{
            rightX,
            row2Y,
            cardWidth,
            cardHeight
        },
        "IMPACT SPEED",
        fixed(impactSpeed, 2) + " m/s",
        "SAFE LIMIT  " +
            fixed(safeImpactSpeed, 2) +
            " m/s",
        impactSpeedSafe ? GREEN : RED
    );

    // Hull integrity card
    Color hullColor = GREEN;

    if (hullIntegrity < 35.0) {
        hullColor = RED;
    }
    else if (hullIntegrity < 70.0) {
        hullColor = YELLOW;
    }

    const Rectangle hullCard{
        leftX,
        row3Y,
        cardWidth,
        cardHeight
    };

    DrawRectangleRec(
        hullCard,
        Fade(Color{12, 20, 34, 255}, 0.90f)
    );

    DrawRectangleLinesEx(
        hullCard,
        2.0f,
        Fade(SKYBLUE, 0.55f)
    );

    DrawText(
        "HULL INTEGRITY",
        static_cast<int>(hullCard.x + 18.0f),
        static_cast<int>(hullCard.y + 13.0f),
        17,
        LIGHTGRAY
    );

    const std::string hullText =
        fixed(hullIntegrity, 0) + "%";

    const int hullTextWidth =
        MeasureText(hullText.c_str(), 30);

    DrawText(
        hullText.c_str(),
        static_cast<int>(
            hullCard.x +
            (hullCard.width - static_cast<float>(hullTextWidth)) / 2.0f
        ),
        static_cast<int>(hullCard.y + 40.0f),
        30,
        hullColor
    );

    drawHullBar(
        Rectangle{
            hullCard.x + 35.0f,
            hullCard.y + 85.0f,
            hullCard.width - 70.0f,
            10.0f
        },
        hullIntegrity,
        hullColor
    );

    // Score card
    drawStatCard(
        Rectangle{
            rightX,
            row3Y,
            cardWidth,
            cardHeight
        },
        "FINAL SCORE",
        std::to_string(score),
        missionSuccessful
            ? "RESCUE COMPLETE"
            : "MISSION INCOMPLETE",
        GOLD
    );

    // Bottom divider
    DrawLine(
        285,
        590,
        995,
        590,
        Fade(SKYBLUE, 0.45f)
    );

    drawCenteredText(
        missionSuccessful
            ? "FLIGHT DATA VERIFIED"
            : "REVIEW FLIGHT DATA BEFORE RETRYING",
        605,
        16,
        GRAY
    );

    // Buttons
    retryButton_2.draw();
    selectMissionButton_2.draw();
    trailButton.draw();
}

}

int main() {

    InitWindow(kScreenWidth, kScreenHeight, "Orbital Rescue - C++ Starter Project");
    SetTargetFPS(60);

    Simulation simulation;
    bool paused = false;

    while (!WindowShouldClose()) {
        ControlInput input;
        if (simulation.getGameStatus() == GameStatus::MainMenu) {

            startButton.update();
            quitButton.update();
            if (quitButton.isClicked()) {
                break;
            }
            if (startButton.isClicked()) {
                simulation.setGameStatus(GameStatus::MissionSelect);
            }
        }else if (simulation.getGameStatus() == GameStatus::Results) {
            retryButton.update();
            statButton.update();
            selectMissionButton.update();
            if (selectMissionButton.isClicked()) {
                simulation.setGameStatus(GameStatus::MissionSelect);
            }else if (retryButton.isClicked()) {
                simulation.setGameStatus(GameStatus::Flight);
                simulation.reset();
            }else if (statButton.isClicked()) {
                simulation.setGameStatus(GameStatus::Stats);
            }
        }else if (simulation.getGameStatus() == GameStatus::Stats) {
            retryButton_2.update();
            selectMissionButton_2.update();
            trailButton.update();
            if (retryButton_2.isClicked()) {
                simulation.setGameStatus(GameStatus::Flight);
                simulation.reset();
            }else if (selectMissionButton_2.isClicked()) {
                simulation.setGameStatus(GameStatus::MissionSelect);
            }else if (trailButton.isClicked()) {
                simulation.setGameStatus(GameStatus::Trail);
            }

        }
        else if (simulation.getGameStatus() == GameStatus::MissionSelect) {
            mission1Button.update();
            mission2Button.update();
            mission3Button.update();
            missionBackButton.update();
            if (mission1Button.isClicked()) {
                loadMissionConfig("./missions/mission_01.txt");
                simulation.setGameStatus(GameStatus::Flight);
                simulation.reset();
            }
            if (mission2Button.isClicked()) {
                loadMissionConfig("./missions/mission_02.txt");
                simulation.setGameStatus(GameStatus::Flight);
                simulation.reset();
            }
            if (mission3Button.isClicked()) {
                loadMissionConfig("./missions/mission_03.txt");
                simulation.setGameStatus(GameStatus::Flight);
                simulation.reset();
            }
            if (missionBackButton.isClicked()) {
                simulation.setGameStatus(GameStatus::MainMenu);
            }
        }else if (simulation.getGameStatus() == GameStatus::Trail) {
            returnButton.update();
            if (returnButton.isClicked()) {
                simulation.setGameStatus(GameStatus::Stats);
            }
        }
        else if (simulation.getGameStatus() == GameStatus::Flight) {
            timescalex1.update();

            timescalex2.update();

            timescalex5.update();

            if (IsKeyPressed(KEY_ENTER)) {
                simulation.start();
            }

            if (IsKeyPressed(KEY_M)) {
                simulation.setGameStatus(GameStatus::MissionSelect);
            }

            if (IsKeyPressed(KEY_COMMA)) {
                simulation.speedDn();
            };

            if (IsKeyPressed(KEY_PERIOD)||timescalex1.isClicked()||timescalex2.isClicked()||timescalex5.isClicked()) {
                simulation.speedUp();
            };

            if (IsKeyPressed(KEY_R)) {
                simulation.reset();
                paused = false;
            }

            if (IsKeyPressed(KEY_P)) {
                paused = !paused;
            }

            if (IsKeyPressed(KEY_Z)) {
                simulation.APEngage();//change the status of AP, turning it on/off
            }

            if (!simulation.APisENGAGED()) {
                input = readControls();
            }else {
                input = simulation.use_AP(simulation.craft());
            }


            if (!paused) {
                const double dt = std::min(GetFrameTime(), 0.033f);
                simulation.update(dt, input);

            }
        }


        if (simulation.getGameStatus() == GameStatus::Flight) {
            BeginDrawing();
            ClearBackground(Color{4, 8, 20, 255});
            drawStars();
            drawStation(simulation.stationPosition(), simulation.portPosition());
            drawSpacecraft(simulation.craft(), input);
            drawTelemetry(simulation);
            drawTimescale(simulation.gettimescale());
            //drawAP(simulation);
            drawPlanet();
            drawHelp();
            drawFPS();
            drawTrail(simulation);
            drawButtons();

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
        }else if (simulation.getGameStatus() == GameStatus::MainMenu) {
            BeginDrawing();
            ClearBackground(Color{4, 8, 20, 255});
            drawMainMenu();
            EndDrawing();
        }else if (simulation.getGameStatus() == GameStatus::MissionSelect) {
            BeginDrawing();
            ClearBackground(Color{4, 8, 20, 255});
            drawMissionSelectMenu();
            EndDrawing();
        }else if (simulation.getGameStatus() == GameStatus::Results) {
            BeginDrawing();
            ClearBackground(Color{4, 8, 20, 255});
            drawResultsMenu(simulation);
            EndDrawing();
        }else if (simulation.getGameStatus() == GameStatus::Stats) {
            BeginDrawing();
            ClearBackground(Color{4, 8, 20, 255});
            MissionStats stats = simulation.getMissionStats();
            drawStatsMenu(simulation.getState()==MissionState::Docked ? true:false,// missionSuccessful

    config_global.name,
    config_global.startFuel,                   // startFuel
    stats.finalFuel,                    // finalFuel
    stats.missionTime,                   // missionTime
    stats.maxSpeed,                   // maxSpeed
    stats.impactSpeed,                    // impactSpeed
    config_global.dockingLimits.safeSpeed,                    // safeImpactSpeed
    stats.hullIntegrity,                    // hullIntegrity
    simulation.score()    );
            EndDrawing();
        }else if (simulation.getGameStatus() == GameStatus::Trail) {
            BeginDrawing();
            ClearBackground(Color{4, 8, 20, 255});
            returnButton.draw();
            drawTrail(simulation);
            drawStars();
            drawPlanet();
            drawStation(simulation.stationPosition(), simulation.portPosition());
            EndDrawing();
        }


    }

    CloseWindow();
    return 0;
}
