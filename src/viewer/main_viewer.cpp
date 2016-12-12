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
        ofstream resp_serialized("/dev/rtp1", ios::binary);

        while(!drawer->isQuit()) {
            WorldData *world = new WorldData();
            cereal::BinaryInputArchive archive(file_serialized);
            archive(*world);

            drawer->updateWorldData(world);
            drawer->drawMap();
            if(drawer->handleEvents()) {
                cereal::BinaryOutputArchive archive(resp_serialized);
                archive(*(drawer->getNewTower()));
            }
        }
    }

    delete drawer;

    return 0;
}