#include "UserInteraction.h"

#include "../WorldState.h"

using namespace std;

UserInteraction::UserInteraction(WorldState* ref) :
        world_state_(ref), interface_(make_unique<UserInteractionInterface>(this))
{

}

void UserInteraction::add_tower(const TowerType& type, const Position<double>& position) {
    requests.push_back({type, position});
}

const std::vector<TowerAddRequest> &UserInteraction::get_tower_add_requests() const {
    return requests;
}

UserInteractionInterface* UserInteraction::get_user_interaction_interface() {
    return interface_.get();
}

void UserInteraction::clear_requests() {
    requests.clear();
}
