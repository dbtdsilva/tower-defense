#ifndef TOWERDEFENSE_TOWERINTERFACE_H
#define TOWERDEFENSE_TOWERINTERFACE_H

#include <helpers/Definitions.h>
#include "../Entity.h"

class Tower;

class TowerInterface : public EntityInterface {
public:
    TowerInterface(Tower* reference);
    void reference_moved(Tower* reference);

    void shoot();
    void radar();
    void rotate(const TowerRotation& rotation);
private:
    Tower* tower_reference_;
};

#endif //TOWERDEFENSE_TOWERINTERFACE_H
