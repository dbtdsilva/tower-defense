#include "Bullet.h"

Bullet::Bullet(const Position<double>& position, const int damage) : pos_(position), damage_(damage) {
}

Position<double>& Bullet::get_position() {
    return pos_;
}
const int& Bullet::get_damage() const {
    return damage_;
}