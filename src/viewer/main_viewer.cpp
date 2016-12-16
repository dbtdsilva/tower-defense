#include <iostream>
#include <fstream>
#include <thread>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include <helpers/WorldDataSerializer.h>
#include <helpers/ViewerDataSerializer.h>
#include "MapDrawer.h"

using namespace std;

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGTH = 800;

void recv_message(MapDrawer *drawer) {
    ifstream file_serialized;
    file_serialized.open("/dev/rtp0", ios::binary);

    char buf[16];
    streamsize bytes_read;
    unsigned long idx, idx_message;
    string value, recv, sub, message;

    while (!drawer->isQuit()) {
        bytes_read = file_serialized.readsome(buf, 16);

        if (bytes_read == 0) 
            continue;

        value = "";
        
        for (int i = 0; i < bytes_read; i++) {
            value += buf[i];
        }
        
        recv += value;
        idx = recv.rfind("MESSAGE");
        
        if (idx <= 0) 
            continue;
        
        sub = recv.substr(0, idx);
        idx_message = sub.rfind("MESSAGE") + 7;
        
        if (idx_message == -1) 
            continue;
        
        message = sub.substr(idx_message);
        recv = recv.substr(idx);

        WorldData *world = new WorldData();

        istringstream filedata(message);
        cereal::BinaryInputArchive archive(filedata);
        archive(*world);

        drawer->updateWorldData(world);
    }
}

void draw_map(MapDrawer *drawer) {
    while(!drawer->isQuit()) {
        drawer->drawMap();
        //SDL_Delay(50);
    }
}

int main() {
    MapDrawer *drawer = new MapDrawer(SCREEN_WIDTH, SCREEN_HEIGTH);

    if(!drawer->initSuccessful()) 
        return -1;

    thread recv_handler(recv_message, drawer);
    thread draw_handler(draw_map, drawer);

    while (!drawer->isQuit()) {
        if(drawer->handleEvents()) {
            OperationTowerData* towerOperation = drawer->getTowerOperation();

            ViewerData dataToSerialize;
            dataToSerialize.status_ = drawer->getGameStatus();
            
            if(towerOperation != nullptr) {
                dataToSerialize.tower_ = *towerOperation;
            }

            ostringstream stream_serialize;
            cereal::BinaryOutputArchive archive(stream_serialize);
            archive(dataToSerialize);

            string serialized_string = "MESSAGE" + stream_serialize.str();
            ofstream pipe_1("/dev/rtp1", ios::binary);

            if(pipe_1) {
                pipe_1 << serialized_string << endl;
                pipe_1.close();
            } else
                cout << "RT_Pipe 1 error" << endl;
        }
    }

    recv_handler.join();

    delete drawer;

    return 0;
}
