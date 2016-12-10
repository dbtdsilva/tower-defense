#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include "logic/WorldState.h"
#include "serializer/WorldDataSerializer.h"


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


    Position<double> pos(2.1,3.4);
    ostringstream stream;
    cereal::BinaryOutputArchive archive(stream);
    archive(pos);

    Position<double> pos2(0,0);
    std::stringstream stream2(stream.str());
    cereal::BinaryInputArchive archive2(stream2);
    archive2(pos2);
    cout << "String: " << pos2;
    return EXIT_SUCCESS;
}
