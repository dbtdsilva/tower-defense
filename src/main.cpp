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


    cout << state.get_data_serialized() << endl;

    WorldData world;
    world.bullets_.push_back(BulletData(Position<double>(2.1, 3.4), 10));
    world.bullets_.push_back(BulletData(Position<double>(4.1, 5.5), 25));
    ostringstream stream;
    cereal::BinaryOutputArchive archive(stream);
    archive(world);

    WorldData world2;
    std::stringstream stream2(stream.str());
    cereal::BinaryInputArchive archive2(stream2);
    archive2(world2);
    cout << "String: " << world2.bullets_.size();
    cout << world2.bullets_[0].position_ << endl;
    cout << world2.bullets_[0].damage_ << endl;
    cout << world2.bullets_[1].position_ << endl;
    cout << world2.bullets_[1].damage_ << endl;
    return EXIT_SUCCESS;
}
