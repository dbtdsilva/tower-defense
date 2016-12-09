#include "UserInteractionAgent.h"
#include "UserInteractionInterface.h"

UserInteractionAgent::UserInteractionAgent(UserInteractionInterface* ref) : interface_(ref) {
}

void UserInteractionAgent::take_action() {
    interface_->add_tower(TowerType::SIMPLE);
}
