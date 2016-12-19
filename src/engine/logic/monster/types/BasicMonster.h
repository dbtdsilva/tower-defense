#ifndef TOWERDEFENSE_BASICMONSTER_H
#define TOWERDEFENSE_BASICMONSTER_H

#include "../../WorldState.h"

// Monster(WorldState* state, const int& health, const double& movement_speed_, const double& rotational_speed,
//      const int& eyes_cost_ms, const Position<double>& pos, const MonsterType& type);
class BasicMonster : public Monster {
public:
    BasicMonster(WorldState* world_ref, const Position<double>& pos) :
            Monster(world_ref, 125, 0.025, (M_PI / 180.0) * 5.0, 100, pos, MonsterType::BASIC) { };
};

#endif //TOWERDEFENSE_BASICMONSTER_H
