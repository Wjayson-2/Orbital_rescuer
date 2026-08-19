#pragma once

#include "Simulation.hpp"
#include "UI.hpp"

namespace input {

struct FrameInput {
    ControlInput controls{};
    bool quitRequested = false;
};

FrameInput handle(Simulation& simulation,
                  ui::UIState& uiState,
                  bool& paused);

} // namespace input
