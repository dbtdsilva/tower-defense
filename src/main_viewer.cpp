#include <iostream>
#include <fstream>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include "serializer/WorldDataSerializer.h"
#include "viewer/MapDrawer.h"

using namespace std;

const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGTH = 600;

int main() {
    ifstream file_serialized("world_serialized.bin", ios::binary);
    WorldData world;
    cereal::BinaryInputArchive archive(file_serialized);
    archive(world);

    MapDrawer drawer(SCREEN_WIDTH, SCREEN_HEIGTH, &world);

    if(drawer.initSuccessful())
        while(1)
            drawer.drawMap();

    return 0;
}