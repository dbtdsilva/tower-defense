#ifndef TOWERDEFENSE_SIMPLETOWER_H
#define TOWERDEFENSE_SIMPLETOWER_H

#include "../TowerData.h"

class SimpleTower : public TowerData {
public:
    SimpleTower(WorldState& world_ref, const Position<double>& pos) :
            TowerData(world_ref, 100, 1, 100, 200, 10, pos) {}
};

#endif //TOWERDEFENSE_SIMPLETOWER_H
