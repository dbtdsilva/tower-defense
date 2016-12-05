#ifndef TOWERDEFENSE_TOWERINTERFACE_H
#define TOWERDEFENSE_TOWERINTERFACE_H

#include "../WorldState.h"

class TowerInterface {
public:
    TowerInterface(WorldState*);

    void shoot();
    void radar();
    void rotate();
private:
    WorldState* world_state_ref_;
};

#endif //TOWERDEFENSE_TOWERINTERFACE_H
