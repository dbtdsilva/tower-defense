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


    std::string sta = "abc";
    int x = 5;
    ostringstream stream;
    cereal::BinaryOutputArchive archive(stream);
    archive(sta, x);

    string b;
    int y;
    std::stringstream stream2(stream.str());
    cereal::BinaryInputArchive archive2(stream2);
    archive2(b, y);
    cout << "String: " << b << ", y: " << y;
    return EXIT_SUCCESS;
}
