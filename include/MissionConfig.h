//
// Created by WJays on 2026/8/3.
//
#ifndef ORBITALRESCUESTARTER_MISSIONCONFIG_H
#define ORBITALRESCUESTARTER_MISSIONCONFIG_H
#include <string>

struct DockingLimits {
    double captureDistance;
    double safeSpeed;
    double safeAngleRadians;
};

struct Vec2
{
    double x;
    double y;
};

struct MissionConfig {
    std::string name;
    std::string callsign;

    Vec2 startPosition;
    Vec2 startVelocity;

    double startAngleRadians;
    double startFuel;
    double timeLimit;

    DockingLimits dockingLimits;

    bool pos_initialized{false};
    bool name_initialized{false};
    bool callsign_initialized{false};
    bool velocity_initialized{false};
    bool angle_initialized{false};
    bool fuel_initialized{false};
    bool time_initialized{false};

};
MissionConfig loadMissionConfig(
    const std::string& filename
);
#endif //ORBITALRESCUESTARTER_MISSIONCONFIG_H
