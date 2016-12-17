#ifndef TOWERDEFENSE_SIMPLETOWER_H
#define TOWERDEFENSE_SIMPLETOWER_H

#include "../Tower.h"

class SimpleTower : public Tower {
public:
    SimpleTower(WorldState* world_ref, const Position<int>& pos) :
            Tower(world_ref, 50, 400, 250, 1.5, 2, pos, TowerType::SIMPLE) {}
};

#endif //TOWERDEFENSE_SIMPLETOWER_H
