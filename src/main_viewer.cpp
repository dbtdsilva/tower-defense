#include <iostream>
#include "viewer/MapDrawer.h"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGTH = 480;

int main() {
    MapDrawer drawer(SCREEN_WIDTH, SCREEN_HEIGTH, nullptr);

    if(drawer.initSuccessful())
        while(1)
            drawer.drawMap();

    return 0;
}