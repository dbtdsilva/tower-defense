#ifndef TOWERDEFENSE_SIMPLETOWER_H
#define TOWERDEFENSE_SIMPLETOWER_H


#include "../Tower.h"

class SimpleTower : public Tower {
public:
    SimpleTower(WorldState& world_ref, const Position<double>& pos);
};


#endif //TOWERDEFENSE_SIMPLETOWER_H
