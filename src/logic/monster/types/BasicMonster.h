#ifndef TOWERDEFENSE_BASICMONSTER_H
#define TOWERDEFENSE_BASICMONSTER_H

#include "../../WorldState.h"

class BasicMonster : public Monster {
    BasicMonster(WorldState& world_ref, const Position<double>& pos);
};

#endif //TOWERDEFENSE_BASICMONSTER_H
