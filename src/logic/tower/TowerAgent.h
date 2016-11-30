#ifndef TOWERDEFENSE_TOWERAGENT_H
#define TOWERDEFENSE_TOWERAGENT_H

#include "Tower.h"

class TowerAgent {
public:
    TowerAgent(Tower&);

    void take_action();
private:
    Tower& tower_ref_;
};


#endif //TOWERDEFENSE_TOWERAGENT_H
