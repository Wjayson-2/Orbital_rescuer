#pragma once
#include "Spacecraft.hpp"
#include <string>
#include <vector>
#include "vec2.hpp"

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
    const Vec2& stationPosition() const { return stationPosition_; }
    const Vec2& portPosition() const { return portPosition_; }

    DockingResult evaluateDocking() const;
    double missionTime() const { return missionTime_; }
    int score() const;

    void start();
    std::string statusMessage() const;


private:
    Spacecraft craft_;
    Vec2 stationPosition_{290.0, 0.0};
    Vec2 portPosition_{290.0-48, 0.0};
    MissionState state_{MissionState::Briefing};
    double missionTime_{0.0};

    void checkDocking();
    void checkFailure();
};

extern std::vector<Vec2> trail_;
