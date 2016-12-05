#include "TowerAgent.h"

TowerAgent::TowerAgent(std::unique_ptr<TowerInterface> ref) : tower_ref_(std::move(ref)) {
}

void TowerAgent::take_action() {
    tower_ref_->radar();
    tower_ref_->rotate();
    tower_ref_->shoot();
}