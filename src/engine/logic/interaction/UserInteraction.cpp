#include "UserInteraction.h"

#include "../WorldState.h"

using namespace std;

UserInteraction::UserInteraction(WorldState* ref) :
        world_state_(ref), interface_(make_unique<UserInteractionInterface>(this))
{

}

void UserInteraction::add_tower(const TowerType& type, const Position<int>& position) {
    requests.push_back({type, position, TowerOperation::INSERT});
}

void UserInteraction::remove_tower(const Position<int>& position) {
    requests.push_back({TowerType::SIMPLE, position, TowerOperation::REMOVE});
}

const std::vector<TowerRequest> &UserInteraction::get_tower_requests() const {
    return requests;
}

UserInteractionInterface* UserInteraction::get_user_interaction_interface() {
    return interface_.get();
}

void UserInteraction::clear_requests() {
    requests.clear();
}
