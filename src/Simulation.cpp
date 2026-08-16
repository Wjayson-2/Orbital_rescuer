#include "Simulation.hpp"
#include <algorithm>
#include <cmath>
#include <array>
#include <iostream>
#include <ostream>
#include <stdexcept>


#include "MissionConfig.hpp"

namespace {



constexpr double kCraftCollisionRadius = 0;

double wrappedAngle(double angle) {
    constexpr double pi = 3.14159265358979323846;
    while (angle > pi) angle -= 2.0 * pi;
    while (angle < -pi) angle += 2.0 * pi;
    return angle;

}
}


void Simulation::recordTrailPoint() {
    trail_[trailNext_] = craft_.position();

    trailNext_ = (trailNext_ + 1) % kMaxTrailPoints;

    if (trailCount_ < kMaxTrailPoints) {
        ++trailCount_;
    }
}

void Simulation::clearTrail() {
    trailNext_ = 0;
    trailCount_ = 0;
}

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
    clearTrail();
    missionStats_.missionTime = missionTime_;
    missionTime_ = 0.0;
    state_ = MissionState::Briefing;
    timeScale_ = TimeScale::x1;
    missionStats_ = {};
    AP_.reset();

}

const Vec2& Simulation::getTrailPoint(std::size_t i) const {
    std::size_t oldest;

    if (trailCount_ < kMaxTrailPoints) {
        oldest = 0;
    } else {
        oldest = trailNext_;
    }

    std::size_t index =
        (oldest + i) % kMaxTrailPoints;

    return trail_[index];
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
    recordTrailPoint();
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
    double craft_vn;
    Vec2 craft_n;

    push_to = craft_.position() + collision_normal * penetration;
    craft_.setPosition(push_to);
    craft_vn = craft_.velocity().x * collision_normal.x + craft_.velocity().y * collision_normal.y;
    craft_n = collision_normal * craft_vn;
    craft_n = craft_n * 2;
    if (craft_vn < 0) {
        craft_.setVelocity(craft_.velocity() - craft_n);
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
        //std::cout<<distance<<std::endl;
        pushback(offset, distance, craft_);
        if (craft_.canTakeCollision()) {

            craft_.applyCollisionDamage(craft_.velocity().magnitude());
            craft_.setCooldown(1.0);
        }
    }
}

ControlInput Simulation::use_AP(Spacecraft& craft_in) {
    if (AP_.isEngaged()) {
         return AP_.steer(craft_in, portPosition());
     }
    return {};
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
