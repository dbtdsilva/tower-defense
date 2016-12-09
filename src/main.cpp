#include <stdlib.h>
#include "logic/WorldState.h"

using namespace std;

int main(int argc, char** argv) {
    WorldState state(10, 10);
    UserInteractionInterface* user = state.get_user_interaction_interface();
    user->add_tower(TowerType::SIMPLE, Position<double>(2,0));
    state.update_world_state();
    cout << state << endl;
    return EXIT_SUCCESS;
}
