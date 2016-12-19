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

    // Each packet has around 572 bytes
    char buffer[2048];
    streamsize bytes_read;
    unsigned long idx, idx_message;
    string raw_received,            // Contains the last raw message received
            raw_total_received,     // Contains the raw messages received till the moment that contain useful info
            useless_chars,          // Used to parse trash characters that might appear
            final_message;          // Final message parsed and ready for the serializer

    while (!drawer->isQuit()) {
        bytes_read = file_serialized.readsome(buffer, 2048);
        if (bytes_read == 0) 
            continue;
        raw_received = string(buffer, bytes_read);
        raw_total_received += raw_received;
        idx = raw_total_received.rfind("MESSAGE");
        if (idx <= 0) 
            continue;

        useless_chars = raw_total_received.substr(0, idx);
        idx_message = useless_chars.rfind("MESSAGE") + 7;
        if (idx_message == -1) 
            continue;

        final_message = useless_chars.substr(idx_message);
        raw_total_received = raw_total_received.substr(idx);

        WorldData *world = new WorldData();

        istringstream file_data(final_message);
        cereal::BinaryInputArchive archive(file_data);
        archive(*world);

        drawer->updateWorldData(world);
    }
}

void draw_map(MapDrawer *drawer) {
    while(!drawer->isQuit()) {
        drawer->drawMap();
    }
}

int main() {
    MapDrawer *drawer = new MapDrawer(SCREEN_WIDTH, SCREEN_HEIGTH);

    if(!drawer->initSuccessful()) 
        return -1;

    thread recv_handler(recv_message, drawer);
    thread draw_handler(draw_map, drawer);

    while (!drawer->isQuit()) {
        if (drawer->handleEvents()) {
            ostringstream stream_serialize;
            cereal::BinaryOutputArchive archive(stream_serialize);
            archive(drawer->getViewerData());

            string serialized_string = stream_serialize.str();
            ofstream pipe_1("/dev/rtp1", ios::binary);

            if(pipe_1) {
                pipe_1 << serialized_string << endl;
                pipe_1.close();
            } else
                cout << "RT_Pipe 1 error" << endl;
        }
    }

    if(recv_handler.joinable())
        recv_handler.join();
    else
        terminate();

    draw_handler.join();

    delete drawer;

    return 0;
}
