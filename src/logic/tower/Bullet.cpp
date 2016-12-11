#include "Bullet.h"

Bullet::Bullet() : position_(0,0), angle_(0), speed_(0), damage_(0) {

}

Bullet::Bullet(const Position<double>& position, const double& angle, const double& speed, const int& damage) :
        position_(position), angle_(angle), speed_(speed), damage_(damage) {

}

Bullet::Bullet(Bullet&& other) : position_(other.position_), angle_(other.angle_), speed_(other.speed_), damage_(other.damage_) {

}

Bullet& Bullet::operator=(Bullet&& other) {
    if (this != &other)
        position_ = other.position_;
    return *this;
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