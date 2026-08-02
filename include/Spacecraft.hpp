#pragma once
#include "Vec2.hpp"

struct ControlInput {
    double mainThrottle{};     // 0.0 to 1.0
    double lateralThrottle{};  // -1.0 to 1.0
    double rotationInput{};    // -1.0 to 1.0
};

class Spacecraft {
public:
    Spacecraft();

    void reset();
    void update(double dt, const ControlInput& input);

    const Vec2& position() const { return position_; }
    const Vec2& velocity() const { return velocity_; }

    double angleRadians() const { return angleRadians_; }
    double angularVelocity() const { return angularVelocity_; }
    double fuel() const { return fuel_; }
    double hullIntegrity() const { return hullIntegrity_; }

    bool hasFuel() const { return fuel_ > 0.0; }
    bool isDestroyed() const { return hullIntegrity_ <= 0.0; }

    void applyCollisionDamage(double relativeSpeed);

private:
    Vec2 position_{};
    Vec2 velocity_{};

    double angleRadians_{};
    double angularVelocity_{};

    double dryMass_{1200.0};
    double fuel_{600.0};
    double hullIntegrity_{100.0};

    static constexpr double kMainThrust = 10000.0;
    static constexpr double kLateralThrust = 2500.0;
    static constexpr double kRotationAcceleration = 1.8;
    static constexpr double kFuelBurnRate = 8.0;

    double totalMass() const { return dryMass_ + fuel_; }
};
