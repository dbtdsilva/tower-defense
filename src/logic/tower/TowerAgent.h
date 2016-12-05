#ifndef TOWERDEFENSE_TOWERAGENT_H
#define TOWERDEFENSE_TOWERAGENT_H

#include <memory>
#include "TowerInterface.h"

class TowerInterface;

class TowerAgent {
public:
    TowerAgent(std::unique_ptr<TowerInterface>);

    void take_action();
private:
    std::unique_ptr<TowerInterface> tower_ref_;
};

#endif //TOWERDEFENSE_TOWERAGENT_H
