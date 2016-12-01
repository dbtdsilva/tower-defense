#include <stdlib.h>
#include "logic/WorldState.h"
#include "logic/WorldUserInteractions.h"

int main(int argc, char** argv) {
    WorldState state;
    WorldUserInteractions interaction(state);

    return EXIT_SUCCESS;
}
