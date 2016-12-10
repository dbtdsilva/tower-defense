#ifndef TOWERDEFENSE_BULLET_H
#define TOWERDEFENSE_BULLET_H

#include "../../helpers/Position.h"

typedef struct {
    Position<double> position;
    const double angle;
    const double speed;
    const int damage;
} Bullet;

#endif //TOWERDEFENSE_BULLET_H
