#include "TowerAgent.h"

TowerAgent::TowerAgent(TowerInterface* ref) : tower_ref_(ref) {
}

void TowerAgent::take_action() {
    tower_ref_->radar();
    tower_ref_->rotate();
    tower_ref_->shoot();
}