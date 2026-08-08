//
// Created by WJays on 2026/8/7.
//
#ifndef ORBITALRESCUESTARTER_AP_H
#define ORBITALRESCUESTARTER_AP_H
#include "Spacecraft.hpp"

enum class AP_status {
    Brake,
    Adjust_x,
    Adjust_y,
    Align,
    Final_dock
};

struct errors {
    double radian_error{};
    double radian_v_error{};
    Vec2 v_error{};
    double x_error{};
    double y_error{};
};

struct AP {
public:
    void engage(bool state);
    bool isEngaged() const {return engaged_;};
    AP_status getStatus() const {return status_;};
    void setStatus(AP_status newStatus) {status_ = newStatus;};
    ControlInput steer(const Spacecraft craft_, const Vec2 portPosition);
    void getErrors(const Spacecraft craft_, const Vec2 portposition);

private:
    AP_status status_{AP_status::Approach};
    bool engaged_ = false;
    errors errors_;

};

#endif //ORBITALRESCUESTARTER_AP_H
