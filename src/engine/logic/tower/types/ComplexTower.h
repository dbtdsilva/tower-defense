#ifndef TOWERDEFENSE_COMPLEXTOWER_H
#define TOWERDEFENSE_COMPLEXTOWER_H

#include "../Tower.h"

class ComplexTower : public Tower {
public:
    ComplexTower(WorldState* world_ref, const Position<int>& pos) :
            Tower(world_ref, 100, 200, 500, 3.0, 2, pos, TowerType::COMPLEX) {}
};

#endif //TOWERDEFENSE_COMPLEXTOWER_H_H
