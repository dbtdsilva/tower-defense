#ifndef TOWERDEFENSE_USERINTERACTIONINTERFACE_H
#define TOWERDEFENSE_USERINTERACTIONINTERFACE_H

#include <helpers/Definitions.h>
#include <helpers/Position.h>
#include "../Entity.h"

class UserInteraction;

class UserInteractionInterface : public EntityInterface {
public:
    UserInteractionInterface(UserInteraction* reference);

    void add_tower(const TowerType& type, const Position<int>& position);
    void remove_tower(const Position<int>& position);
private:
    UserInteraction* user_interaction_;
};


#endif //TOWERDEFENSE_USERINTERACTIONINTERFACE_H
