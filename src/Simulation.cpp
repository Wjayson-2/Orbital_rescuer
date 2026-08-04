#include "Simulation.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <vector>

#include "MissionConfig.h"

namespace {

constexpr int kMaxTrailPoints = 300;
double wrappedAngle(double angle) {
    constexpr double pi = 3.14159265358979323846;
    while (angle > pi) angle -= 2.0 * pi;
    while (angle < -pi) angle += 2.0 * pi;
    return angle;

}
}

std::vector<Vec2> trail_;

Simulation::Simulation() = default;

void Simulation::reset() {

    craft_.reset();
    trail_.clear();
    missionTime_ = 0.0;
    state_ = MissionState::Briefing;
}

void Simulation::start() {
    if (state_ == MissionState::Briefing) {
        state_ = MissionState::Running;
    }
}


void Simulation::update(double dt, const ControlInput& input) {
    if (state_ != MissionState::Running) {
        return;
    }
    if (trail_.size() < kMaxTrailPoints) {
        trail_.push_back(craft_.position());
    }else if (trail_.size() == kMaxTrailPoints) {
        trail_.erase(trail_.begin());
    }else {
        throw std::logic_error("Simulation::update(): Trail size too large");
    }
    missionTime_ += dt;
    craft_.update(dt, input);

    checkDocking();
    checkFailure();
}

DockingResult Simulation::evaluateDocking() const {
    const Vec2 offset = craft_.position() - stationPosition_;
    const double distance = offset.magnitude();
    const double speed = craft_.velocity().magnitude();

    // The station's docking axis points left, so the craft should point right.
    const double angleError = std::abs(wrappedAngle(craft_.angleRadians()));
    return {
        distance <= config_global.dockingLimits.captureDistance,
        speed <= config_global.dockingLimits.safeSpeed,
        angleError <= config_global.dockingLimits.safeAngleRadians,
    };
}

void Simulation::checkDocking() {
    const DockingResult result = evaluateDocking();
    if (!result.insideCaptureZone) {
        return;
    }

    if (result.speedSafe && result.angleSafe) {
        state_ = MissionState::Docked;
        return;
    }

    craft_.applyCollisionDamage(craft_.velocity().magnitude());

    // Bounce away from the station after a failed docking attempt.
    craft_.update(0.05, ControlInput{});
}

void Simulation::checkFailure() {
    const Vec2 p = craft_.position();

    if (craft_.isDestroyed() || std::abs(p.x) > 1000.0 || std::abs(p.y) > 700.0) {
        state_ = MissionState::Failed;
    }

    if (missionTime_ > 180.0) {
        state_ = MissionState::Failed;
    }
}

int Simulation::score() const {
    if (state_ != MissionState::Docked) {
        return 0;
    }

    const int fuelScore = static_cast<int>(craft_.fuel() * 2.0);
    const int timeScore = std::max(0, 1200 - static_cast<int>(missionTime_ * 8.0));
    const int hullScore = static_cast<int>(craft_.hullIntegrity() * 10.0);

    return fuelScore + timeScore + hullScore;
}

std::string Simulation::statusMessage() const {
    switch (state_) {
        case MissionState::Briefing:
            return "PRESS ENTER TO START";
        case MissionState::Running:
            return "APPROACH THE STATION";
        case MissionState::Docked:
            return "DOCKING COMPLETE";
        case MissionState::Failed:
            return "MISSION FAILED - PRESS R";
    }
    return {};
}
