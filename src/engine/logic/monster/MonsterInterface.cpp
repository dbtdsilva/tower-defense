#include "MonsterInterface.h"
#include "Monster.h"

MonsterInterface::MonsterInterface(Monster* reference) : monster_reference_(reference) {

}

void MonsterInterface::reference_moved(Monster* reference) {
    monster_reference_ = reference;
}

std::vector<MonsterEye> MonsterInterface::eyes() {
    return monster_reference_->eyes();
}

void MonsterInterface::move(const MonsterMovement& movement) {
    return monster_reference_->move(movement);
}

void MonsterInterface::rotate(const MonsterRotation& rotation) {
    return monster_reference_->rotate(rotation);
}

const unsigned int MonsterInterface::get_identifier() const {
    return monster_reference_->get_identifier();
}