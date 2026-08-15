//
// Created by WJays on 2026/8/9.
//
#include "WrapAngle.hpp"

#include <numbers>

double wrapAngle(double angle) {
    constexpr double pi = std::numbers::pi_v<double>;

    while (angle > pi) angle -= 2.0 * pi;
    while (angle < -pi) angle += 2.0 * pi;

    return angle;
}
