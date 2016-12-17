#ifndef TOWERDEFENSE_BULLET_H
#define TOWERDEFENSE_BULLET_H

#include <helpers/Position.h>

class Bullet {
public:
    Bullet();
    Bullet(const Position<double>& position, const double& angle, const double& speed, const int& damage,
           const double& range);
    Bullet(const Bullet&) = delete;
    Bullet(Bullet&& other);
    Bullet& operator=(const Bullet& other) = delete;
    Bullet& operator=(Bullet&& other) = delete;

    Position<double>& get_position();
    const Position<double>& get_position() const;
    const double& get_angle() const;
    const double& get_speed() const;
    const double& get_range() const;
    const int& get_damage() const;
    double get_distance_travelled() const;
private:
    Position<double> position_;
    const Position<double> initial_position_;
    const double angle_, speed_, range_;
    const int damage_;
};

#endif //TOWERDEFENSE_BULLET_H
