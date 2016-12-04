#ifndef TOWERDEFENSE_MONSTERAGENT_H
#define TOWERDEFENSE_MONSTERAGENT_H

#include "Monster.h"

class MonsterAgent {
public:
    MonsterAgent(Monster&);

    void take_action();
private:
    Monster& monster_ref_;
};

#endif //TOWERDEFENSE_MONSTERAGENT_H
