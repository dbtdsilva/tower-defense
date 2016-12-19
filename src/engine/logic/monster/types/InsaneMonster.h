#ifndef TOWERDEFENSE_INSANEMONSTER_H
#define TOWERDEFENSE_INSANEMONSTER_H

#include "../../WorldState.h"

// Monster(WorldState* state, const int& health, const double& movement_speed_, const double& rotational_speed,
//      const int& eyes_cost_ms, const Position<double>& pos, const MonsterType& type);
class InsaneMonster : public Monster {
public:
    InsaneMonster(WorldState* world_ref, const Position<double>& pos) :
            Monster(world_ref, 250, 0.035, (M_PI / 180.0) * 10.0, 100, pos, MonsterType::INSANE) { };
};

#endif //TOWERDEFENSE_INSANEMONSTER_H
