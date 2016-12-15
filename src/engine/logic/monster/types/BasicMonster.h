#ifndef TOWERDEFENSE_BASICMONSTER_H
#define TOWERDEFENSE_BASICMONSTER_H

#include "../../WorldState.h"

class BasicMonster : public Monster {
public:
    BasicMonster(WorldState* world_ref, const Position<double>& pos) :
            Monster(world_ref, 100, 0.025, (M_PI / 180.0) * 5.0, pos, MonsterType::BASIC) { };
};

#endif //TOWERDEFENSE_BASICMONSTER_H
