#include "Input.hpp"

#include "raylib.h"
#include "MissionConfig.hpp"

namespace input {
namespace {

ControlInput readManualControls() {
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

void handleMainMenu(Simulation& simulation,
                    ui::UIState& uiState,
                    FrameInput& result) {
    uiState.startButton.update();
    uiState.quitButton.update();

    if (uiState.quitButton.isClicked()) {
        result.quitRequested = true;
    }

    if (uiState.startButton.isClicked()) {
        simulation.setGameStatus(GameStatus::MissionSelect);
    }
}

void handleResults(Simulation& simulation, ui::UIState& uiState) {
    uiState.retryButton.update();
    uiState.statButton.update();
    uiState.selectMissionButton.update();

    if (uiState.selectMissionButton.isClicked()) {
        simulation.setGameStatus(GameStatus::MissionSelect);
    } else if (uiState.retryButton.isClicked()) {
        simulation.setGameStatus(GameStatus::Flight);
        simulation.reset();
    } else if (uiState.statButton.isClicked()) {
        simulation.setGameStatus(GameStatus::Stats);
    }
}

void handleStats(Simulation& simulation, ui::UIState& uiState) {
    uiState.retryButton_2.update();
    uiState.selectMissionButton_2.update();
    uiState.trailButton.update();

    if (uiState.retryButton_2.isClicked()) {
        simulation.setGameStatus(GameStatus::Flight);
        simulation.reset();
    } else if (uiState.selectMissionButton_2.isClicked()) {
        simulation.setGameStatus(GameStatus::MissionSelect);
    } else if (uiState.trailButton.isClicked()) {
        simulation.setGameStatus(GameStatus::Trail);
    }
}

void startMission(Simulation& simulation, const char* path) {
    loadMissionConfig(path);
    simulation.setGameStatus(GameStatus::Flight);
    simulation.reset();
}

void handleMissionSelect(Simulation& simulation, ui::UIState& uiState) {
    uiState.mission1Button.update();
    uiState.mission2Button.update();
    uiState.mission3Button.update();
    uiState.missionBackButton.update();

    if (uiState.mission1Button.isClicked()) {
        startMission(simulation, "./missions/mission_01.txt");
    } else if (uiState.mission2Button.isClicked()) {
        startMission(simulation, "./missions/mission_02.txt");
    } else if (uiState.mission3Button.isClicked()) {
        startMission(simulation, "./missions/mission_03.txt");
    } else if (uiState.missionBackButton.isClicked()) {
        simulation.setGameStatus(GameStatus::MainMenu);
    }
}

void handleTrail(Simulation& simulation, ui::UIState& uiState) {
    uiState.returnButton.update();

    if (uiState.returnButton.isClicked()) {
        simulation.setGameStatus(GameStatus::Stats);
    }
}

void handleFlight(Simulation& simulation,
                  ui::UIState& uiState,
                  bool& paused,
                  FrameInput& result) {
    uiState.timescalex1.update();
    uiState.timescalex2.update();
    uiState.timescalex5.update();

    if (IsKeyPressed(KEY_ENTER)) {
        simulation.start();
    }

    if (IsKeyPressed(KEY_M)) {
        simulation.setGameStatus(GameStatus::MissionSelect);
        return;
    }

    if (IsKeyPressed(KEY_COMMA)) {
        simulation.speedDn();
    }

    if (IsKeyPressed(KEY_PERIOD) ||
        uiState.timescalex1.isClicked() ||
        uiState.timescalex2.isClicked() ||
        uiState.timescalex5.isClicked()) {
        simulation.speedUp();
    }

    if (IsKeyPressed(KEY_R)) {
        simulation.reset();
        paused = false;
    }

    if (IsKeyPressed(KEY_P)) {
        paused = !paused;
    }

    if (IsKeyPressed(KEY_Z)) {
        simulation.APEngage();
    }

    if (simulation.APisENGAGED()) {
        result.controls = simulation.use_AP(simulation.craft());
    } else {
        result.controls = readManualControls();
    }
}

} // namespace

FrameInput handle(Simulation& simulation,
                  ui::UIState& uiState,
                  bool& paused) {
    FrameInput result;

    switch (simulation.getGameStatus()) {
        case GameStatus::MainMenu:
            handleMainMenu(simulation, uiState, result);
            break;

        case GameStatus::Results:
            handleResults(simulation, uiState);
            break;

        case GameStatus::Stats:
            handleStats(simulation, uiState);
            break;

        case GameStatus::MissionSelect:
            handleMissionSelect(simulation, uiState);
            break;

        case GameStatus::Trail:
            handleTrail(simulation, uiState);
            break;

        case GameStatus::Flight:
            handleFlight(simulation, uiState, paused, result);
            break;
    }

    return result;
}

} // namespace input
