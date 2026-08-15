//
// Created by WJays on 2026/8/3.
//
#ifndef ORBITALRESCUESTARTER_MISSIONCONFIG_H
#define ORBITALRESCUESTARTER_MISSIONCONFIG_H
#include <string>
#include "Vec2.hpp"

struct DockingLimits {
    double captureDistance{};
    double safeSpeed{};
    double safeAngleRadians{};
};


struct Planet {
    Vec2 position{};
    double radius{};
    double mass{};
};

struct MissionConfig {
    std::string name{};
    std::string callsign{};

    Vec2 startPosition{};
    Vec2 startVelocity{};

    double startAngleRadians{};
    double startFuel{};
    double timeLimit{};
    double mass{};
    double angularVelocity{};
    double hullIntegrity{};

    DockingLimits dockingLimits{};

    Planet planetspec{};

    bool pos_initialized{false};
    bool name_initialized{false};
    bool callsign_initialized{false};
    bool velocity_initialized{false};
    bool angle_initialized{false};
    bool fuel_initialized{false};
    bool time_initialized{false};
    bool angularVelocity_initialized{false};
    bool hullIntegrity_initialized{false};
    bool dockingLimits_initialized{false};
    bool planetspec_initialized{false};
    bool mass_initialized{false};

};
void loadMissionConfig(
    const std::string& filename
);
extern MissionConfig config_global;
#endif //ORBITALRESCUESTARTER_MISSIONCONFIG_H
