#include <stdlib.h>
#include <iostream>
#include "logic/WorldState.h"

using namespace std;

int main(int argc, char** argv) {
    WorldState state(10, 10);
    UserInteractionInterface* user = state.get_user_interaction_interface();
    user->add_tower(TowerType::SIMPLE, Position<int>(2,0));
    vector<EntityModification> changes = state.update_world_state();
    for (EntityModification& change : changes) {
        cout << "Action: " << (int)change.action_ << endl;
    }
    cout << state << endl;
    return EXIT_SUCCESS;
}
