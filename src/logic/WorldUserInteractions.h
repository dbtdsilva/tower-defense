#ifndef TOWERDEFENSE_WORLDUSERINTERACTIONS_H
#define TOWERDEFENSE_WORLDUSERINTERACTIONS_H


#include "WorldState.h"

class WorldUserInteractions {
public:
    WorldUserInteractions(WorldState& ref);

    bool add_tower();
private:
    WorldState& world_ref_;
};


#endif //TOWERDEFENSE_WORLDUSERINTERACTIONS_H
