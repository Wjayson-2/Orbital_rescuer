#include "AP.h"

#include "MissionConfig.h"
#include "Simulation.hpp"
#include "Vec2.hpp"
#include "wrapAngle.h"

void AP::engage() {
    bool state;
    state = engaged_;
    engaged_ = !state;
}

ControlInput AP::steer(const Spacecraft craft_, const Vec2 portPosition) {
    ControlInput input;
    Vec2 port = portPosition; //port location
    Vec2 target_approach = port; //the approaching target, first bind it to the port.
    target_approach.x -= (config_global.dockingLimits.captureDistance + 100);
    getErrors(craft_, portPosition);
    if (status_ == AP_status::Approach) {
        double kp = 1.5; //Rotation coefficient
        Vec2 position_error = target_approach - craft_.position();
        double desired_angle = atan2(position_error.y, position_error.x);
        double radian_error = wrapAngle(desired_angle - craft_.angleRadians());//makes sure radian_error is minimized to -180-180 deg
        double rotation_input = kp * radian_error - kp * craft_.angularVelocity();//combines both radian error and current angularv
        input.rotationInput += rotation_input;

    }
    return input;

}

void AP::getErrors(const Spacecraft craft_, const Vec2 portPosition) {
    errors errors_;
    Vec2 port = portPosition;
    port.x -= config_global.dockingLimits.captureDistance; // The location of the port
    errors_.x_error = craft_.position().x - port.x;
    errors_.y_error = craft_.position().y - port.y;
    errors_.radian_error = craft_.angleRadians();
    errors_.radian_v_error = craft_.angularVelocity() - config_global.dockingLimits.safeAngleRadians;
    errors_.v_error.x = (craft_.velocity().x - config_global.dockingLimits.safeSpeed);
    errors_.v_error.y = craft_.velocity().y;
}


//
// if (IsKeyDown(KEY_W)) {
//     input.mainThrottle = 1.0;
// }
// if (IsKeyDown(KEY_D)) {
//     input.lateralThrottle -= 1.0;
// }
// if (IsKeyDown(KEY_A)) {
//     input.lateralThrottle += 1.0;
// }
// if (IsKeyDown(KEY_E)) {
//     input.rotationInput -= 1.0;
// }
// if (IsKeyDown(KEY_Q)) {
//     input.rotationInput += 1.0;
// }
