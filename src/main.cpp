#include <stdlib.h>
#include "logic/WorldState.h"

using namespace std;

int main(int argc, char** argv) {
    WorldState state(10, 10);

    cout << state << endl;
    return EXIT_SUCCESS;
}
