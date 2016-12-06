#ifndef TOWERDEFENSE_MONSTERAGENT_H
#define TOWERDEFENSE_MONSTERAGENT_H

#include "MonsterInterface.h"

class MonsterInterface;

class MonsterAgent {
public:
    MonsterAgent(std::unique_ptr<MonsterInterface>);

    void take_action();
private:
    std::unique_ptr<MonsterInterface> monster_ref_;
};

#endif //TOWERDEFENSE_MONSTERAGENT_H
