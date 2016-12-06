//
// Created by myrddin on 06/12/16.
//

#ifndef TOWERDEFENSE_MONSTERINTERFACE_H
#define TOWERDEFENSE_MONSTERINTERFACE_H

#include "Monster.h"

class Monster;

class MonsterInterface {
public:
    MonsterInterface(Monster* reference);

    void move();
    void left_eye();
    void middle_eye();
    void right_eye();
    void rotate();
private:
    Monster* monster_reference_;
};


#endif //TOWERDEFENSE_MONSTERINTERFACE_H
