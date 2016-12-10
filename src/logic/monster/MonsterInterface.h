#ifndef TOWERDEFENSE_MONSTERINTERFACE_H
#define TOWERDEFENSE_MONSTERINTERFACE_H

#include <vector>
#include "../helpers/Definitions.h"
#include "MonsterEye.h"
#include "../helpers/Entity.h"

class Monster;

class MonsterInterface : public EntityInterface {
public:
    MonsterInterface(Monster* reference);

    std::vector<MonsterEye> eyes();
    void move(const MonsterMovement&);
    void rotate(const MonsterRotation&);
private:
    Monster* monster_reference_;
};


#endif //TOWERDEFENSE_MONSTERINTERFACE_H
