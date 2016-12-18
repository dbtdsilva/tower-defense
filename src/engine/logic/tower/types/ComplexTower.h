#ifndef TOWERDEFENSE_COMPLEXTOWER_H
#define TOWERDEFENSE_COMPLEXTOWER_H

#include "../Tower.h"

// Tower(WorldState* state, const int& damage, const int& weapon_load_cycles, const int& radar_load_time,
//     const int& cost, const double& range, const double& rotational_speed, const Position<int>& pos,
//     const TowerType& type)

class ComplexTower : public Tower {
public:
    ComplexTower(WorldState* world_ref, const Position<int>& pos) :
            Tower(world_ref, 100, 10, 100, 500, 3.0, M_PI / 20.0, pos, TowerType::COMPLEX) {}
};

#endif //TOWERDEFENSE_COMPLEXTOWER_H_H
