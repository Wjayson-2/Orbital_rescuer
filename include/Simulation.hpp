#pragma once
#include "Spacecraft.hpp"
#include <string>
#include <vector>
#include "vec2.hpp"
#include "AP.h"

enum class TimeScale {
    x1,
    x2,
    x5
};

enum class GameStatus {
    MainMenu,
    MissionSelect,
    Flight,
    Results,
    Stats,
    Trail
};

struct MissionStats {
    double finalFuel{};
    double maxSpeed{};
    double impactSpeed{};
    double hullIntegrity{};
    double missionTime{};
};

enum class MissionState {
    Briefing,
    Running,
    Docked,
    Failed
};

struct DockingResult {
    bool insideCaptureZone{};
    bool speedSafe{};
    bool angleSafe{};
};

class Simulation {
public:
    Simulation();

    void reset();
    void update(double dt, const ControlInput& input);

    const Spacecraft& craft() const { return craft_; }
    Spacecraft& craft() { return craft_; }

    MissionState state() const { return state_; }
    TimeScale gettimescale() const { return timeScale_; }
    const Vec2& stationPosition() const { return stationPosition_; }
    const Vec2& portPosition() const { return portPosition_; }

    DockingResult evaluateDocking() const;
    double missionTime() const { return missionTime_; }
    int score() const;

    void start();
    void speedUp();
    void speedDn();
    std::string statusMessage() const;

    const GameStatus getGameStatus() const {return gameStatus_; }
    void setGameStatus(GameStatus newStatus);
    void updateStat();
    const MissionStats getMissionStats() const {return missionStats_;}
    const MissionState getState() const {return state_;}
    ControlInput use_AP(Spacecraft craft_);
    const bool APisENGAGED() const {return AP_.isEngaged();}

private:
    MissionStats missionStats_;
    AP AP_;
    TimeScale timeScale_;
    Spacecraft craft_;
    GameStatus gameStatus_{GameStatus::MainMenu};//progress of the game
    Vec2 stationPosition_{290.0, 0.0};
    Vec2 portPosition_{290.0-48, 0.0};
    MissionState state_{MissionState::Briefing};//state of ship in mission
    double missionTime_{0.0};

    void checkDocking();
    void checkFailure();
};

extern std::vector<Vec2> trail_;
