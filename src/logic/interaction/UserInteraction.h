#ifndef TOWERDEFENSE_USERINTERACTION_H
#define TOWERDEFENSE_USERINTERACTION_H

#include <memory>
#include "UserInteractionInterface.h"
#include "UserInteractionAgent.h"

class WorldState;

class UserInteraction {
public:
    UserInteraction(WorldState*);

    void add_tower(const TowerType& type);
private:
    std::unique_ptr<UserInteractionInterface> interface_;
    std::unique_ptr<UserInteractionAgent> agent_;

    WorldState* world_state_;
};


#endif //TOWERDEFENSE_USERINTERACTION_H
