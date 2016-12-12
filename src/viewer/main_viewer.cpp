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
    ifstream file_serialized;
    file_serialized.open("/dev/rtp0", ios::binary);

    char buf[16];
    int bytes_read, idx, idx_message;
    string value, recv, sub, message;
    while (true) {
        bytes_read = file_serialized.readsome(buf, 16);

        if (bytes_read == 0) continue;
        value = "";
        for (int i = 0; i < bytes_read; i++) {
            value += (char) buf[i];
        }
        recv += value;

        idx = recv.rfind("MESSAGE");
        if (idx <= 0) continue;
        sub = recv.substr(0, idx);

        idx_message = sub.rfind("MESSAGE") + 7;
        if (idx_message == -1) continue;
        message = sub.substr(idx_message);
        recv = recv.substr(idx);

        WorldData *world = new WorldData();

        istringstream filedata(message);
        cereal::BinaryInputArchive archive(filedata);
        archive(*world);

        cout << world->monsters_.size() << endl; 
    }

    return 0;
}
