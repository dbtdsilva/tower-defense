#include "TowerAgent.h"

#include "TowerInterface.h"

TowerAgent::TowerAgent(TowerInterface* ref) : tower_interface_(ref) {
}

void TowerAgent::take_action() {
    tower_interface_->radar();
    tower_interface_->rotate(TowerRotation::LEFT);
    tower_interface_->shoot();
}