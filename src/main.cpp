#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "logic/WorldState.h"
#include "serializer/WorldDataSerializer.h"

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

    ofstream world_serialize;
    world_serialize.open("world_serialized.bin", ios::binary);
    state.serialize_data(world_serialize);
    world_serialize.close();

    return EXIT_SUCCESS;
}
