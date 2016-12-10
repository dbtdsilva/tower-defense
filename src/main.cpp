#include <stdlib.h>
#include <iostream>

#include "logic/WorldState.h"

using namespace std;

int main(int argc, char** argv) {
    WorldState state(10, 10);
    UserInteractionInterface* user = state.get_user_interaction_interface();
    user->add_tower(TowerType::SIMPLE, Position<int>(2,0));
    cout << state << endl;
    for (int i = 0; i < 40; i++) {
        cout << i << endl;
        vector<EntityModification> changes = state.update_world_state();
        for (EntityModification& change : changes) {
            TowerInterface* tower;
            MonsterInterface* monster;
            cout << "Action: " << (int)change.action_ << endl;
            if ((monster = dynamic_cast<MonsterInterface*>(change.entity_)) != nullptr) {
                cout << "Monster" << endl;
            } else if ((tower = dynamic_cast<TowerInterface*>(change.entity_)) != nullptr) {
                cout << "Tower" << endl;
                tower->radar();
                tower->shoot();
            }
        }
    }
    state.get_data_serialized()

    return EXIT_SUCCESS;
}
