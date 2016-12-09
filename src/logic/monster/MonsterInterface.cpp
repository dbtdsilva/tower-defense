#include "MonsterInterface.h"
#include "Monster.h"

MonsterInterface::MonsterInterface(Monster* reference) : monster_reference_(reference) {

}

void MonsterInterface::move() {
    monster_reference_->move();
}

void MonsterInterface::left_eye() {
    monster_reference_->left_eye();
}

void MonsterInterface::middle_eye() {
    monster_reference_->middle_eye();
}

void MonsterInterface::right_eye() {
    monster_reference_->right_eye();
}

void MonsterInterface::rotate() {
    monster_reference_->rotate();
}