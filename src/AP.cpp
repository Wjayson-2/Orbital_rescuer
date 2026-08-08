#include "AP.h"

#include "MissionConfig.h"
#include "Simulation.h"
#include "Vec2.hpp"

void AP::engage(bool state) {
    engaged_ = state;
}

ControlInput AP::steer(Spacecraft craft_, Vec2 portPosition) {
    ControlInput input;
    getErrors(craft_, portPosition);
}

void AP::getErrors(Spacecraft craft_, Vec2 portPosition) {
    errors errors_;
    Vec2 port = portPosition;
    port.x -= config_global.dockingLimits(); // The location of the port
    errors_.x_error = craft_.position().x - port.x;
    errors_.y_error = craft_.position().y - port.y;
}

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
