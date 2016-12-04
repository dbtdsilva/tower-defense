#include "TowerAgent.h"

TowerAgent::TowerAgent(Tower& ref) : tower_ref_(ref) {
}

void TowerAgent::take_action() {
    std::vector<Monster> monsters_in_vision = tower_ref_.radar();

    if (monsters_in_vision.size() != 0)
        tower_ref_.shoot();
}