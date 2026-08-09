//
// Created by WJays on 2026/8/9.
//
#include "wrapAngle.h"
double wrapAngle(double angle) {
    constexpr double pi = 3.14159265358979323846;

    while (angle > pi) angle -= 2.0 * pi;
    while (angle < -pi) angle += 2.0 * pi;

    return angle;
}