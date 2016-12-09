#include "Monster.h"
#include "types/BasicMonster.h"

using namespace std;

Monster Monster::add_monster(WorldState* world_ref, const MonsterType& ref, const Position<double>& position) {
    switch (ref) {
        case MonsterType::BASIC:
            return BasicMonster(world_ref, position);
    }
}

Monster::Monster(WorldState* state, const int& health, const double& movement_speed_, const double& rotational_speed,
    const Position<double>& pos) :
        world_ref_(state), health_(health), movement_speed_(movement_speed_), rotational_speed_(rotational_speed),
        pos_(pos), angle_(0), interface_(make_unique<MonsterInterface>(this))
{
}

Monster::~Monster() {
}

Monster::Monster(Monster&& other) :
        world_ref_(other.world_ref_), health_(other.health_), movement_speed_(movement_speed_),
        rotational_speed_(other.rotational_speed_), pos_(other.pos_), angle_(0),
        interface_(std::move(other.interface_))
{
    other.interface_ = nullptr;
}


const Position<double>& Monster::get_position() const {
    return pos_;
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