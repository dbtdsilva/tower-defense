#include "Monster.h"

Monster Monster::add_monster(WorldState& world_ref, const MonsterType& ref, const Position<double>& position) {
    switch (ref) {
        case BASIC:
            //return SimpleTower(world_ref, position);
            break;
    }
}

Monster::Monster(WorldState& state, const int& health, const double& movement_speed_, const double& rotational_speed,
    const Position<double>& pos) :
        world_ref_(state), health_(health), movement_speed_(movement_speed_), rotational_speed_(rotational_speed),
        pos_(pos), angle_(0)
{
}