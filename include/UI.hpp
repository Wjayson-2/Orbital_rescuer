#pragma once

#include "raylib.h"
#include "AppConstants.hpp"
#include "Buttons.hpp"
#include "Simulation.hpp"

namespace ui {

struct UIState {
    Button timescalex1{
        .bounds = Rectangle{1190, kScreenHeight - 92, 70, 70},
        .text = "x1",
        .fontSize = 56,
        .visible = true
    };

    Button timescalex2{
        .bounds = Rectangle{1190, kScreenHeight - 92, 70, 70},
        .text = "x2",
        .fontSize = 56,
        .visible = false
    };

    Button timescalex5{
        .bounds = Rectangle{1190, kScreenHeight - 92, 70, 70},
        .text = "x5",
        .fontSize = 56,
        .visible = false
    };

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

    Button returnButton{
        .bounds = Rectangle{520.0f, 640.0f, 240.0f, 52.0f},
        .text = "RETURN",
        .fontSize = 22,
        .visible = true
    };
};

void draw(const Simulation& simulation,
          const ControlInput& controlInput,
          bool paused,
          UIState& state);

} // namespace ui
