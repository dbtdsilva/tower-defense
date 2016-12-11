#ifndef TOWERDEFENSE_BULLET_H
#define TOWERDEFENSE_BULLET_H

#include "../../helpers/Position.h"

class Bullet {
public:
    Bullet();
    Bullet(const Position<double>& position, const double& angle, const double& speed, const int& damage);
    Bullet(const Bullet&) = delete;
    Bullet(Bullet&& other);
    Bullet& operator=(const Bullet& other) = delete;
    Bullet& operator=(Bullet&& other);

    Position<double>& get_position();
    const Position<double>& get_position() const;
    const double& get_angle() const;
    const double& get_speed() const;
    const int& get_damage() const;
private:
    Position<double> position_;
    const double angle_;
    const double speed_;
    const int damage_;
};

#endif //TOWERDEFENSE_BULLET_H
