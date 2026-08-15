//
// Created by WJays on 2026/8/7.
//
#ifndef ORBITALRESCUESTARTER_AP_H
#define ORBITALRESCUESTARTER_AP_H
#include "Spacecraft.hpp"

enum class AP_status {
    Approach,
    Brake,
    Align,
    Final_approach,
    Docked,
    Abort
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
    void engage();
    bool isEngaged() const {return engaged_;};
    AP_status getStatus() const {return status_;};
    void setStatus(AP_status newStatus) {status_ = newStatus;};
    ControlInput steer(const Spacecraft& craft_, const Vec2& portPosition);
    void getErrors(const Spacecraft& craft_, const Vec2& portposition);

private:
    AP_status status_{AP_status::Approach};
    bool engaged_ {false};
    errors errors_;
    bool isMaxRecorded_ {false};
    double max_distance_ {0};
    double max_radian_error_ {0};
    double max_y_distance_ {0};
    bool finished_ {false}; //tells whether the lateral brake should work

};

#endif //ORBITALRESCUESTARTER_AP_H
