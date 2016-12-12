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
    string value, recv;
    while (true) {
        bytes_read = file_serialized.readsome(buf, 16);
        if (bytes_read == 0) continue;
        string value;
        for (int i = 0; i < bytes_read; i++) {
            value += (char) buf[i];
        }
        
        cout << value.size() << endl;
        recv += value;

        idx = recv.rfind("MESSAGE");
        if (idx <= 0) continue;
        string sub = recv.substr(0, idx);

        idx_message = sub.rfind("MESSAGE") + 7;
        if (idx_message == -1) continue;
        string message = sub.substr(idx_message);
        recv = recv.substr(idx);

        cout << "line: " << message << endl;
        printf("%d %d %d\n", message[0], message.size(), sizeof(message.c_str()));
        WorldData *world = new WorldData();
        
        istringstream filedata(message);
        cereal::BinaryInputArchive archive(filedata);
        archive(*world);

        cout << world->monsters_.size() << endl; 
    }
    /*int pipe_fd;
    int counter = 0;
    char devname[32], buf[16];

    sprintf(devname, "/dev/rtp%d", 0);
    pipe_fd = open(devname, O_RDWR);
    
    if (pipe_fd < 0) {
        printf("It failed!\n");
        return -1;
    }

    while(read(pipe_fd, buf, sizeof(buf)) >= 0) {
        printf("(id = %d) %s\n", counter++, buf);  
        recv += string(buf);
        cout << recv << endl;

        idx = recv.rfind("MESSAGE");
        if (idx <= 0) continue;
        string sub = recv.substr(0, idx);

        idx_message = sub.rfind("MESSAGE") + 7;
        if (idx_message == -1) continue;
        string message = sub.substr(idx_message);
        recv = recv.substr(idx);

        cout << "line: " << message << endl;
        printf("%d %d\n", message[0], message.size());
        WorldData *world = new WorldData();
        ifstream filedata(message.c_str());
        cereal::BinaryInputArchive archive(filedata);
        archive(*world);

        cout << world->monsters_.size() << endl; 
    }

    

    printf("wat:.\n");
    if(true) {
        cout << "wat" << endl;
        ifstream file_serialized("/dev/rtp0");
        cout << "wat" << endl;
        //ofstream resp_serialized("/dev/rtp1", ios::binary);
        cout << "wat" << endl;

        string line;
        string recv;

        int idx, idx_message;
        while(true) {
            getline(file_serialized, line);
            recv += line;
            cout << recv << endl;

            idx = recv.rfind("MESSAGE");
            if (idx <= 0) continue;
            string sub = recv.substr(0, idx);

            idx_message = sub.rfind("MESSAGE") + 7;
            if (idx_message == -1) continue;
            string message = sub.substr(idx_message);
            recv = recv.substr(idx);

            cout << "line: " << message << endl;
            printf("%d %d\n", message[0], message.size());
            WorldData *world = new WorldData();
            ifstream filedata(message.c_str());
            cereal::BinaryInputArchive archive(filedata);
            archive(*world);

            cout << world->monsters_.size() << endl;

            drawer->updateWorldData(world);
            drawer->drawMap();
            if(drawer->handleEvents()) {
                cereal::BinaryOutputArchive archive(resp_serialized);
                archive(*(drawer->getNewTower()));
            }

            //while (file_serialized.gcount() == 0);
        }
    }

    //delete drawer;*/

    return 0;
}
