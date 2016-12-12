#include <iostream>
#include <fstream>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include <helpers/WorldDataSerializer.h>
#include "MapDrawer.h"

using namespace std;

const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGTH = 600;

int main() {
    ifstream file_serialized("world_serialized.bin", ios::binary);
    WorldData *world = new WorldData();
    cereal::BinaryInputArchive archive(file_serialized);
    archive(*world);
    file_serialized.close();

    MapDrawer *drawer = new MapDrawer(SCREEN_WIDTH, SCREEN_HEIGTH, world);

    if(drawer->initSuccessful())
        while(!drawer->isQuit()) {
            drawer->drawMap();
            drawer->handleEvents();
        }

    delete drawer;

    return 0;
}