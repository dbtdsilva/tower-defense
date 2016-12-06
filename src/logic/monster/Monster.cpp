#include "Monster.h"
#include "types/BasicMonster.h"

using namespace std;

Monster Monster::add_monster(WorldState* world_ref, const MonsterType& ref, const Position<double>& position) {
    switch (ref) {
        case BASIC:
            return BasicMonster(world_ref, position);
    }
}

Monster::Monster(WorldState* state, const int& health, const double& movement_speed_, const double& rotational_speed,
    const Position<double>& pos) :
        world_ref_(state), health_(health), movement_speed_(movement_speed_), rotational_speed_(rotational_speed),
        pos_(pos), angle_(0), agent_(make_unique<MonsterAgent>(make_unique<MonsterInterface>(this)))
{
}

void Monster::move() {

}

void Monster::left_eye() {

}

void Monster::middle_eye() {

}

void Monster::right_eye() {

}

void Monster::rotate() {

}