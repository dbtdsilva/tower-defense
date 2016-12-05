#ifndef TOWERDEFENSE_TOWERAGENT_H
#define TOWERDEFENSE_TOWERAGENT_H

#include "TowerInterface.h"

class TowerAgent {
public:
    TowerAgent(TowerInterface*);

    void take_action();
private:
    TowerInterface* tower_ref_;
};


#endif //TOWERDEFENSE_TOWERAGENT_H
