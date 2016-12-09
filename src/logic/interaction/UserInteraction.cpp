#include "UserInteraction.h"

using namespace std;

UserInteraction::UserInteraction(WorldState* ref) :
        world_state_(ref), interface_(make_unique<UserInteractionInterface>(this)),
        agent_(make_unique<UserInteractionAgent>(interface_.get()))
{

}

void UserInteraction::add_tower(const TowerType& type) {
    //world_state_->add_tower(type);
}