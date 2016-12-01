#ifndef TOWERDEFENSE_BULLET_H
#define TOWERDEFENSE_BULLET_H

#include "../helpers/Position.h"

class Bullet {
public:
    Bullet(const Position<double>&, const int damage);
    Position<double>& get_position();
    const int& get_damage() const;
private:
    const int damage_;
    Position<double> pos_;
};


#endif //TOWERDEFENSE_BULLET_H
