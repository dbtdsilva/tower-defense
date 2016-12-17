#include "UserInteractionInterface.h"
#include "UserInteraction.h"

UserInteractionInterface::UserInteractionInterface(UserInteraction* reference) : user_interaction_(reference) {
}

void UserInteractionInterface::add_tower(const TowerType& type, const Position<int>& position) {
    user_interaction_->add_tower(type, position);
}

void UserInteractionInterface::remove_tower(const Position<int>& position) {
    user_interaction_->remove_tower(position);
}