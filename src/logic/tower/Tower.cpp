#include "Tower.h"
#include "types/SimpleTower.h"

using namespace std;

unsigned int Tower::instance_counter = 0;

Tower Tower::create_tower(WorldState* world_ref, const TowerType& ref, const Position<double>& position) {
    switch (ref) {
        case SIMPLE:
            return SimpleTower(world_ref, position);
    }
}

Tower::Tower(WorldState* state, const int& damage, const int& radar_load_time, const int& cost, const int& range,
             const double& rotational_speed, const Position<double>& pos) :
        damage_(damage), radar_load_time_(radar_load_time), cost_(cost), range_(range),
        rotational_speed_(rotational_speed), pos_(pos), angle_(0), world_ref_(state),
        agent_(make_unique<TowerAgent>(make_unique<TowerInterface>(this))), id_(instance_counter)
{
    Tower::instance_counter++;
}

const int& Tower::get_cost() const {
    return cost_;
}

void Tower::shoot() {

}

void Tower::radar() {

}

void Tower::rotate() {

}