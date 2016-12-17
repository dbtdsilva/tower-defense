#include "Bullet.h"
#include <cmath>

Bullet::Bullet() : position_(0,0), angle_(0), speed_(0), damage_(0), range_(0), initial_position_(position_) {

}

Bullet::Bullet(const Position<double>& position, const double& angle,
               const double& speed, const int& damage, const double& range) :
        position_(position), angle_(angle), speed_(speed), damage_(damage), initial_position_(position), range_(range) {

}

Bullet::Bullet(Bullet&& other) : position_(other.position_), angle_(other.angle_), speed_(other.speed_),
                                 damage_(other.damage_), initial_position_(other.position_), range_(other.range_) {

}

Position<double>& Bullet::get_position() {
    return position_;
}

const Position<double>& Bullet::get_position() const {
    return position_;
}

const double& Bullet::get_angle() const {
    return angle_;
}

const double& Bullet::get_speed() const {
    return speed_;
}

const int& Bullet::get_damage() const {
    return damage_;
}

double Bullet::get_distance_travelled() const {
    return sqrt(pow(initial_position_.get_x() - position_.get_x(), 2) +
                pow(initial_position_.get_y() - position_.get_y(), 2));
}

const double &Bullet::get_range() const {
    return range_;
}
