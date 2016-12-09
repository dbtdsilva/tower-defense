#include <stdlib.h>
#include "logic/WorldState.h"

using namespace std;

int main(int argc, char** argv) {
    WorldState state(10, 10);

    UserInteractionAgent* user = state.get_user_interaction_agent();
    user->take_action();
    state.update_world_state();
    cout << state << endl;
    return EXIT_SUCCESS;
}
