#pragma once
#include <cmath>

struct Vec2 {
    double x{};
    double y{};

    Vec2 operator+(const Vec2& rhs) const { return {x + rhs.x, y + rhs.y}; }
    Vec2 operator-(const Vec2& rhs) const { return {x - rhs.x, y - rhs.y}; }
    Vec2 operator*(double scalar) const { return {x * scalar, y * scalar}; }
    Vec2 operator/(double scalar) const { return {x / scalar, y / scalar}; }

    Vec2& operator+=(const Vec2& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    double magnitude() const { return std::sqrt(x * x + y * y); }

    Vec2 normalized() const {
        const double m = magnitude();
        return m > 1e-9 ? *this / m : Vec2{};
    }
};

inline double dot(const Vec2& a, const Vec2& b) {
    return a.x * b.x + a.y * b.y;
}
