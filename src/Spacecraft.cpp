#include "Spacecraft.hpp"
#include <algorithm>
#include <cmath>

Spacecraft::Spacecraft() {
    reset();
}

void Spacecraft::reset() {
    position_ = {-260.0, 80.0};
    velocity_ = {10.0, -2.0};
    angleRadians_ = 0.15;
    angularVelocity_ = 0.0;
    fuel_ = 600.0;
    hullIntegrity_ = 100.0;
}

void Spacecraft::update(double dt, const ControlInput& input) {
    if (isDestroyed()) {
        return;
    }

    const double mainThrottle = std::clamp(input.mainThrottle, 0.0, 1.0);
    const double lateralThrottle = std::clamp(input.lateralThrottle, -1.0, 1.0);
    const double rotationInput = std::clamp(input.rotationInput, -1.0, 1.0);

    const Vec2 forward{std::cos(angleRadians_), std::sin(angleRadians_)};
    const Vec2 right{-forward.y, forward.x};

    Vec2 force{};

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

void Spacecraft::applyCollisionDamage(double relativeSpeed) {
    if (relativeSpeed <= 4.0) {
        return;
    }

    const double damage = (relativeSpeed - 4.0) * 7.5;
    hullIntegrity_ = std::max(0.0, hullIntegrity_ - damage);
}
