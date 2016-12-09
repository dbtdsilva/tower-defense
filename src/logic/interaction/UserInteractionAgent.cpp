#include "UserInteractionAgent.h"
#include "UserInteractionInterface.h"
#include "../helpers/Position.h"

UserInteractionAgent::UserInteractionAgent(UserInteractionInterface* ref) : interface_(ref) {
}

void UserInteractionAgent::take_action() {
    interface_->add_tower(TowerType::SIMPLE, Position<double>(0,0));
}
