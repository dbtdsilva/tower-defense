#include "Tower.h"

#include <cmath>
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

vector<Position<double>> Tower::radar() {
    // Implement a cost function

    // Return the value
    vector<Position<double>> monsters_in_range;
    for (const Monster& monster : world_ref_->get_monsters()) {
        const Position<double>& monster_position = monster.get_position();
        if (sqrt(pow(monster_position.get_x() - pos_.get_x(), 2) + pow(monster_position.get_y() - pos_.get_y(), 2)) <
                range_) {
            monsters_in_range.push_back(monster_position);
        }
    }
    return monsters_in_range;
}

void Tower::rotate() {

}