#ifndef TOWERDEFENSE_TOWERAGENT_H
#define TOWERDEFENSE_TOWERAGENT_H

#include <memory>

class TowerInterface;

class TowerAgent {
public:
    TowerAgent(TowerInterface* tower_interface_);

    void take_action();
private:
    TowerInterface* tower_interface_;
};

#endif //TOWERDEFENSE_TOWERAGENT_H
