#include "TowerData.h"
#include "types/SimpleTower.h"

TowerData TowerData::create_tower(WorldState& world_ref, const TowerType& ref, const Position<double>& position) {
    switch (ref) {
        case SIMPLE:
            return SimpleTower(world_ref, position);
    }
}

TowerData::TowerData(WorldState& state, const int& damage, const int& radar_load_time, const int& cost, const int& range,
             const double& rotational_speed, const Position<double>& pos) :
        damage_(damage), radar_load_time_(radar_load_time), cost_(cost), range_(range),
        rotational_speed_(rotational_speed), pos_(pos), angle_(0), world_ref_(state)
{
}

const int& TowerData::get_cost() const {
    return cost_;
}