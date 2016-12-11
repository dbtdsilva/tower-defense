#include <fstream>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include "serializer/WorldDataSerializer.h"

using namespace std;

int main() {
    ifstream file_serialized("world_serialized.bin", ios::binary);
    WorldData world;
    cereal::BinaryInputArchive archive(file_serialized);
    archive(world);

    cout << world.monsters_.size() << endl;
    return 0;
}
