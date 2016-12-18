#ifndef TOWERDEFENSE_SIMPLETOWER_H
#define TOWERDEFENSE_SIMPLETOWER_H

#include "../Tower.h"

// Tower(WorldState* state, const int& damage, const int& weapon_load_cycles, const int& radar_load_time,
//     const int& cost, const double& range, const double& rotational_speed, const Position<int>& pos,
//     const TowerType& type)

class SimpleTower : public Tower {
public:
    SimpleTower(WorldState* world_ref, const Position<int>& pos) :
            Tower(world_ref, 50, 15, 200, 250, 1.5, M_PI / 20.0, pos, TowerType::SIMPLE) {}
};

#endif //TOWERDEFENSE_SIMPLETOWER_H
