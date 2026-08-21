#include "Spacecraft.hpp"
#include <algorithm>
#include <cmath>
#include "MissionConfig.hpp"

Spacecraft::Spacecraft() {
    reset();
}



void Spacecraft::reset() {
    position_ = {config_global.startPosition.x, config_global.startPosition.y};
    velocity_ = {config_global.startVelocity.x, config_global.startVelocity.y};
    angleRadians_ = config_global.startAngleRadians;
    angularVelocity_ = config_global.angularVelocity;
    hullIntegrity_ = config_global.hullIntegrity;
    fuel_ = config_global.startFuel;
    dryMass_ = config_global.mass;
    setCooldown(0.0);

}

void Spacecraft::setPosition(const Vec2& position) {
    position_ = position;
}

void Spacecraft::setVelocity(const Vec2& velocity) {
    velocity_ = velocity;
}

void Spacecraft::update(double dt, const ControlInput& input) {
    if (isDestroyed()) {
        return;
    }
    if (canTakeCollision()) {
        registerCollision(dt);
    }
    const double mainThrottle = std::clamp(input.mainThrottle, 0.0, 1.0);
    const double lateralThrottle = std::clamp(input.lateralThrottle, -1.0, 1.0);
    const double rotationInput = std::clamp(input.rotationInput, -1.0, 1.0);

    const Vec2 forward{std::cos(angleRadians_), std::sin(angleRadians_)};
    const Vec2 right{-forward.y, forward.x};

    double Distance;

    const double kG = 6.67430e-11; //gravitational constant

    Distance = (position_ - config_global.planetspec.position).magnitude(); //distance between planet and ship

    double gravity_magnitude;

    if (Distance > config_global.planetspec.radius) {
        gravity_magnitude = (kG * config_global.planetspec.mass)/pow(Distance, 2);
    }else {
        gravity_magnitude = (kG * config_global.planetspec.mass)/pow(config_global.planetspec.radius, 2);
    }

    Vec2 gravitational_pull = (-position_ + config_global.planetspec.position).normalized() * gravity_magnitude; //direction of gravity

    Vec2 force{};

    force += gravitational_pull;

    if (hasFuel()) {
        force += forward * (kMainThrust * mainThrottle);
        force += right * (kLateralThrust * lateralThrottle);

        const double fuelUsed =
            (mainThrottle + 0.35 * std::abs(lateralThrottle)
             + 0.15 * std::abs(rotationInput))
            * kFuelBurnRate * dt;

        fuel_ = std::max(0.0, fuel_ - fuelUsed);
        angularVelocity_ += rotationInput * kRotationAcceleration * dt;
    }

    // Very weak drag helps the simulation remain controllable.
    force += velocity_ * -0.35;

    const Vec2 acceleration = force / totalMass();
    velocity_ += acceleration * dt;
    position_ += velocity_ * dt;

    angularVelocity_ *= std::pow(0.96, dt * 60.0);
    angleRadians_ += angularVelocity_ * dt;
}


bool Spacecraft::canTakeCollision() const {
    if (cooldown_ <= 0) {
        return true;
    }else {
        return false;
    }
};

void Spacecraft::registerCollision(double dt) {
    cooldown_ -= dt;
}

void Spacecraft::setCooldown(double cooldown) {
    cooldown_ = cooldown;
}

void Spacecraft::applyCollisionDamage(double relativeSpeed) {
    if (relativeSpeed <= config_global.dockingLimits.safeSpeed) {
        return;
    }

    const double damage = (relativeSpeed- config_global.dockingLimits.safeSpeed) * 0.75;
    hullIntegrity_ = std::max(0.0, hullIntegrity_ - damage);

}
