#include "UserInteraction.h"

#include "../WorldState.h"

using namespace std;

UserInteraction::UserInteraction(WorldState* ref) :
        world_state_(ref), interface_(make_unique<UserInteractionInterface>(this)),
        agent_(make_unique<UserInteractionAgent>(interface_.get()))
{

}

void UserInteraction::add_tower(const TowerType& type, const Position<double>& position) {
    requests.push_back({type, position});
}

const std::vector<TowerAddRequest> &UserInteraction::get_tower_add_requests() const {
    return requests;
}

UserInteractionAgent* UserInteraction::get_user_interaction_agent() {
    return agent_.get();
}

void UserInteraction::clear_requests() {
    requests.clear();
}
