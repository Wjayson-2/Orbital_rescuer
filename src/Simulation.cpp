#include "Simulation.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <vector>

#include "MissionConfig.h"

namespace {

constexpr int kMaxTrailPoints = 30000;

constexpr double kCraftCollisionRadius = 0;

double wrappedAngle(double angle) {
    constexpr double pi = 3.14159265358979323846;
    while (angle > pi) angle -= 2.0 * pi;
    while (angle < -pi) angle += 2.0 * pi;
    return angle;

}
}

std::vector<Vec2> trail_;

Simulation::Simulation() = default;

void Simulation::setGameStatus(GameStatus newStatus) {
    gameStatus_ = newStatus;
}

void Simulation::speedUp() {
    if (timeScale_ == TimeScale::x1) {
        timeScale_ = TimeScale::x2;
    }else if (timeScale_ == TimeScale::x2) {
        timeScale_ = TimeScale::x5;
    }else {
        timeScale_ = TimeScale::x1;
    }
}

void Simulation::speedDn() {
    if (timeScale_ == TimeScale::x5) {
        timeScale_ = TimeScale::x2;
    }else if (timeScale_ == TimeScale::x2) {
        timeScale_ = TimeScale::x1;
    }else {
        timeScale_ = TimeScale::x5;
    }
}

// void Simulation::updateStat(Simulation simulation) {
//     if (missionStats_.maxSpeed < simulation.craft_.velocity().magnitude()) {
//         missionStats_.maxSpeed = simulation.craft_.velocity().magnitude();
//     }
//     missionStats_.finalFuel = simulation.craft_.fuel();
//     missionStats_.impactSpeed = simulation.craft_.velocity().magnitude();
//     missionStats_.hullIntegrity = simulation.craft_.hullIntegrity();
//     missionStats_.missionTime = simulation.missionTime();
//
// }

void Simulation::updateStat() {
    if (missionStats_.maxSpeed < craft_.velocity().magnitude()) {
        missionStats_.maxSpeed = craft_.velocity().magnitude();
    }
    missionStats_.finalFuel = craft_.fuel();
    missionStats_.impactSpeed = craft_.velocity().magnitude();
    missionStats_.hullIntegrity = craft_.hullIntegrity();
    missionStats_.missionTime = missionTime();

}

void Simulation::reset() {

    craft_.reset();
    trail_.clear();
    missionStats_.missionTime = missionTime_;
    missionTime_ = 0.0;
    state_ = MissionState::Briefing;
    timeScale_ = TimeScale::x1;
    AP_.setStatus(AP_status::Approach);
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
    int time_scale;
    if (timeScale_ == TimeScale::x5) {
        time_scale = 5;
    }else if (timeScale_ == TimeScale::x2) {
        time_scale = 2;
    }else {
        time_scale = 1;
    }

    for (int i = 0; i < time_scale; ++i) {
        craft_.update(dt, input);
        missionTime_ += dt;
        updateStat();
        checkDocking();
        checkFailure();

        if (state_ != MissionState::Running) {
            break;
        }
    }
}

void pushback(Vec2 offset, double distance, Spacecraft& craft_) {
    double penetration = (config_global.dockingLimits.captureDistance + kCraftCollisionRadius) - distance;
    Vec2 push_to;
    Vec2 collision_normal = offset.normalized();
    Vec2 push_v;

    push_to = craft_.position() + collision_normal * penetration;
    craft_.setPosition(push_to);
    push_v = ((-craft_.velocity()).normalized()) * craft_.velocity().magnitude();
    if (dot(craft_.velocity().normalized(), craft_.velocity()) > 0.0) {
        craft_.setVelocity(push_v);
    }

}

DockingResult Simulation::evaluateDocking() const {
    const Vec2 offset = craft_.position() - portPosition();
    const double distance = offset.magnitude();
    const double speed = craft_.velocity().magnitude();



    // The station's docking axis points left, so the craft should point right.
    const double angleError = std::abs(wrappedAngle(craft_.angleRadians()));

    const double collisionDistance = config_global.dockingLimits.captureDistance + kCraftCollisionRadius;

    return {
        distance <= collisionDistance,
        speed <= config_global.dockingLimits.safeSpeed,
        angleError <= config_global.dockingLimits.safeAngleRadians,
    };
}



void Simulation::checkDocking() {
    const Vec2 offset = craft_.position() - portPosition();
    const double distance = offset.magnitude();
    const DockingResult result = evaluateDocking();
    if (!result.insideCaptureZone) {
        return;
    }

    if (result.speedSafe && result.angleSafe) {
        state_ = MissionState::Docked;
        setGameStatus(GameStatus::Results);
        return;
    }


    if (distance < config_global.dockingLimits.captureDistance) {
        std::cout<<distance<<std::endl;
        pushback(offset, distance, craft_);
        if (craft_.cooldown_<=0) {

            craft_.applyCollisionDamage(craft_.velocity().magnitude());
            craft_.cooldown_ = 1.0;
        }
    }
}

ControlInput Simulation::use_AP(Spacecraft craft_) {
     if (AP_.isEngaged()) {
         return AP_.steer(craft_, portPosition());
     }
}

void Simulation::APEngage() {
    AP_.engage();
}

void Simulation::checkFailure() {
    const Vec2 p = craft_.position();
    double Distance = (craft_.position() - config_global.planetspec.position).magnitude();

    if (craft_.isDestroyed() || std::abs(p.x) > 1000.0 || std::abs(p.y) > 700.0) {
        state_ = MissionState::Failed;
        setGameStatus(GameStatus::Results);
    }else if (missionTime_ > config_global.timeLimit) {
        state_ = MissionState::Failed;
        setGameStatus(GameStatus::Results);
    }else if (Distance < config_global.planetspec.radius) {
        missionStats_.hullIntegrity = 0;
        state_ = MissionState::Failed;
        setGameStatus(GameStatus::Results);
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

std::string Simulation::statusMessage() const{
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
