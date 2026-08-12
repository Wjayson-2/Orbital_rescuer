#include "AP.h"

#include <algorithm>
#include <iostream>
#include <ostream>

#include "MissionConfig.h"
#include "Simulation.hpp"
#include "Vec2.hpp"
#include "wrapAngle.h"

void AP::engage() {
    bool state;
    state = engaged_;
    engaged_ = !state;
}

double double_abs(double number) {
    if (number < 0) {
        number = -number;
    }
    return number;
}

bool isMaxRecorded = false;
double max_distance = 0;
double max_radian_error = 0;
double max_y_distance = 0;
bool finished = false; //tells whether the lateral brake should work

ControlInput AP::steer(const Spacecraft craft_, const Vec2 portPosition) {
    ControlInput input;
    Vec2 port = portPosition; //port location
    Vec2 target_approach = port; //the approaching target, first bind it to the port.
    target_approach.x -= (config_global.dockingLimits.captureDistance + 200);
    double kp = 1.5; //Rotation coefficient
    //getErrors(craft_, portPosition);
    if (status_ == AP_status::Approach) {
        Vec2 position_error = target_approach - craft_.position();
        double desired_angle = atan2(position_error.y, position_error.x);
        double radian_error = wrapAngle(desired_angle - craft_.angleRadians());//makes sure radian_error is minimized to -180-180 deg
        bool need_help = false;
        finished = false;
        if (!isMaxRecorded) {
            max_distance = (target_approach - craft_.position()).magnitude(); //Find the max distance for future normalization
            max_radian_error = radian_error;
            isMaxRecorded = true;
        }

        double kt = std::clamp(std::min((radian_error / max_radian_error) , (craft_.velocity().magnitude() / 7)), 0.1, 0.7); //Thrust coefficient, sth that coorporates the radian_error and distance to the thrust provided

        // std::cout << kt << std::endl;
        // std::cout << position_error.magnitude() << std::endl;
        if (double_abs(radian_error) < 5 && position_error.magnitude() >= 10 && craft_.velocity().magnitude() > 11) {
            radian_error = atan2(-craft_.velocity().y, -craft_.velocity().x);
            double rotation_input = kp * radian_error - kp * craft_.angularVelocity();//combines both radian error and current angular v
            input.rotationInput += rotation_input;
            kt = 1;
            double thrust_input = kt * position_error.magnitude() / max_distance;
            input.mainThrottle += std::clamp(thrust_input, 0.0, 1.0);
            need_help = true;
        }

        if (double_abs(radian_error) < 5 && position_error.magnitude() >= 10 && craft_.velocity().magnitude() <= 10) { //10 is the maximum allowed v
            double thrust_input = kt * position_error.magnitude() / max_distance;
            input.mainThrottle += std::clamp(thrust_input, 0.0, 1.0);
        };
        if (double_abs(radian_error) > 0.01 && !need_help) {
            double rotation_input = kp * radian_error - kp * craft_.angularVelocity();//combines both radian error and current angular v
            input.rotationInput += rotation_input;
        }


        //std::cout << position_error.x <<position_error.y << std::endl;
        if (double_abs(position_error.x) < 150 && double_abs(position_error.y) < 5) {
            status_ = AP_status::Brake;
            isMaxRecorded = false;
        }
    }else if (status_ == AP_status::Brake) {
        double velocity_heading = atan2(-craft_.velocity().y, -craft_.velocity().x); //measures the current v heading such that brake can be done
        double desired_angle = velocity_heading;
        double radian_error = wrapAngle(desired_angle - craft_.angleRadians());
        if (double_abs(radian_error) > 0.01) {
            double rotation_input = kp * radian_error - kp * craft_.angularVelocity();//combines both radian error and current angular v
            input.rotationInput += rotation_input;
        } else {
            double kbrake = 10; //the thrust coefficient when braking
            double velocity_error = double_abs(craft_.velocity().magnitude());
            double thrust_input = velocity_error / 10 * kbrake;
            input.mainThrottle += std::clamp(thrust_input, 0.0, 1.0);
        }
        if (craft_.velocity().magnitude() < 0.01) {
            status_ = AP_status::Align;
        }

    }else if (status_ == AP_status::Align) {
        double desired_angle = 0;
        Vec2 position_error = port - craft_.position();
        double radian_error = wrapAngle(desired_angle - craft_.angleRadians());//makes sure radian_error is minimized to -180-180 deg
        // std::cout<<radian_error<<std::endl;
        if (double_abs(radian_error) > 0.01) {
            double rotation_input = kp * radian_error - kp * craft_.angularVelocity();//combines both radian error and current angular v
            input.rotationInput += rotation_input;
        } else if (double_abs(port.y - craft_.position().y) > config_global.dockingLimits.captureDistance || double_abs(craft_.velocity().y) > 0.01) {
            if (!isMaxRecorded) {
                max_y_distance = double_abs(port.y - craft_.position().y);
            }
          double error_y = port.y - craft_.position().y;
            if (craft_.velocity().magnitude() < 2 && !finished) {
                input.lateralThrottle += std::clamp(error_y / max_y_distance, -0.7, 0.7);
            }
            //std::cout<<position_error.x<<" "<<position_error.y<<" "<<error_y<<std::endl;
            if  (double_abs(position_error.y) < 5 && double_abs(craft_.velocity().y) > 0) {
                double klateralbrake = -10; //the thrust coefficient when braking
                double velocity_error = (craft_.velocity().y);
                double thrust_input = velocity_error / 5 * klateralbrake;
                input.lateralThrottle += std::clamp(thrust_input, -1.0, 1.0);
                finished = true;
            }
        } else {
            status_ = AP_status::Final_approach;
            double max_distance = (target_approach - craft_.position()).magnitude();
            isMaxRecorded = false;
        }
    }else if (status_ == AP_status::Final_approach) {
        Vec2 position_error = port - craft_.position();
        double desired_angle = atan2(position_error.y, position_error.x);
        double radian_error = wrapAngle(desired_angle - craft_.angleRadians() );//makes sure radian_error is minimized to -180-180 deg
        // std::cout<<radian_error<<std::endl;
        double kt = std::clamp(std::min((position_error.magnitude() / max_distance) , (craft_.velocity().magnitude() / (config_global.dockingLimits.safeSpeed * 2 / 5))), 0.1, 0.3); //Thrust coefficient, sth that coorporates the radian_error and distance to the thrust provided
        // std::cout<< max_distance<<std::endl;
        if (double_abs(radian_error) < 5 && position_error.magnitude() >= 10 && craft_.velocity().magnitude() <= config_global.dockingLimits.safeSpeed * 3 / 4 ) { //10 is the maximum allowed v
            double thrust_input = kt * position_error.magnitude() / max_distance;
            input.mainThrottle += std::clamp(thrust_input, 0.0, 1.0);
        }


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
