#include "types/SimpleTower.h"
#include "Tower.h"

using namespace std;

Tower Tower::create_tower(WorldState& world_ref, const TowerType& ref, const Position<double>& position) {
    switch (ref) {
        case SIMPLE:
            return SimpleTower(world_ref, position);
    }
}

Tower::Tower(WorldState& state, const int& damage, const int& radar_load_time, const int& cost, const int& range,
             const double& rotational_speed, const Position<double>& pos) :
    damage_(damage), radar_load_time_(radar_load_time), cost_(cost), range_(range),
    rotational_speed_(rotational_speed), pos_(pos), angle_(0), world_ref_(state)
{
}

void Tower::shoot() {
    world_ref_.tower_shoot(Bullet(pos_, damage_));
}

std::vector<Monster> Tower::radar() {
    return world_ref_.tower_radar();
}

const int& Tower::get_cost() const {
    return cost_;
}