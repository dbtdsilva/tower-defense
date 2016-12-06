#ifndef TOWERDEFENSE_BASICMONSTER_H
#define TOWERDEFENSE_BASICMONSTER_H

#include "../../WorldState.h"

class BasicMonster : public Monster {
public:
    BasicMonster(WorldState* world_ref, const Position<double>& pos) : Monster(world_ref, 100, 1000, 1000, pos) { };
};

#endif //TOWERDEFENSE_BASICMONSTER_H
