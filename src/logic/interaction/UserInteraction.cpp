#include "UserInteraction.h"

UserInteraction::UserInteraction(std::unique_ptr<UserInteractionInterface> interface) :
        interface_(std::move(interface)) {

}

void UserInteraction::take_action() {

}