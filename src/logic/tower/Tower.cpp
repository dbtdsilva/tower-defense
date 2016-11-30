#include <logic/tower/types/SimpleTower.h>
#include "Tower.h"

using namespace std;

unique_ptr<Tower> Tower::create_tower(const TowerType& ref, const Position<double>& position) {
    std::unique_ptr<Tower> new_tower = nullptr;
    switch (ref) {
        case SIMPLE:
            new_tower = make_unique<SimpleTower>(position);
            break;
    }
    return new_tower;
}

Tower::Tower(const int& damage, const int& radar_load_time, const int& cost, const int& range,
             const double& rotational_speed, const Position<double>& pos) :
    damage_(damage), radar_load_time_(radar_load_time), cost_(cost), range_(range),
    rotational_speed_(rotational_speed), pos_(pos), angle_(0)
{
}

void Tower::shoot() {

}

