#ifndef TOWERDEFENSE_USERINTERACTIONINTERFACE_H
#define TOWERDEFENSE_USERINTERACTIONINTERFACE_H

#include "../WorldState.h"

class WorldState;

class UserInteractionInterface {
public:
    UserInteractionInterface(WorldState* reference);

    void add_tower();
private:
    WorldState* world_reference_;
};


#endif //TOWERDEFENSE_USERINTERACTIONINTERFACE_H
