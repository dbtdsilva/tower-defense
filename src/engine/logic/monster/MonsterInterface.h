#ifndef TOWERDEFENSE_MONSTERINTERFACE_H
#define TOWERDEFENSE_MONSTERINTERFACE_H

#include <vector>
#include <helpers/Definitions.h>
#include "../Entity.h"
#include "MonsterEye.h"

class Monster;

class MonsterInterface : public EntityInterface {
public:
    MonsterInterface(Monster* reference);
    void reference_moved(Monster* reference);

    std::vector<MonsterEye> eyes();
    void move(const MonsterMovement&);
    void rotate(const MonsterRotation&);
private:
    Monster* monster_reference_;
};


#endif //TOWERDEFENSE_MONSTERINTERFACE_H
