#ifndef TOWERDEFENSE_TOWERINTERFACE_H
#define TOWERDEFENSE_TOWERINTERFACE_H

#include <helpers/Definitions.h>
#include <helpers/Position.h>
#include "../Entity.h"

class Tower;

class TowerInterface : public EntityInterface {
public:
    TowerInterface(Tower* reference);
    void reference_moved(Tower* reference);

    void shoot();
    const std::vector<Position<double>> radar() const;
    void rotate(const TowerRotation& rotation);
    const Position<int> get_position() const;
    const double get_angle() const;
private:
    Tower* tower_reference_;
};

#endif //TOWERDEFENSE_TOWERINTERFACE_H
