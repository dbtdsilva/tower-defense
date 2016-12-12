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
    MapDrawer *drawer = new MapDrawer(SCREEN_WIDTH, SCREEN_HEIGTH);

    if(drawer->initSuccessful()) {
        ifstream file_serialized("/dev/rtp0", ios::binary);

        while(!drawer->isQuit()) {
            //ifstream file_serialized("world_serialized.bin", ios::binary);
            WorldData *world = new WorldData();
            cereal::BinaryInputArchive archive(file_serialized);
            archive(*world);

            drawer->updateWorldData(world);
            drawer->drawMap();
            if(drawer->handleEvents()) {
                std::cout << drawer->getNewTower()->position_.get_x() << std::endl;
            }
        }
    }

    delete drawer;

    return 0;
}