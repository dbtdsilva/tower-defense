#include "Tower.h"

Tower::Tower(const int& damage, const int& radar_cooldown_time, const int& cost, const int& range,
             const double& rotational_speed, const Position<double>& pos) :
    damage_(damage), radar_cooldown_time_(radar_cooldown_time), cost_(cost), range_(range),
    rotational_speed_(rotational_speed), pos_(pos), angle_(0)
{
}

void Tower::shoot() {

}

