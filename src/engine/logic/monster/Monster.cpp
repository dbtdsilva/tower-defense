#include "Monster.h"

#include <cmath>
#include "types/BasicMonster.h"

using namespace std;

unsigned int Monster::instance_counter = 0;

Monster Monster::add_monster(WorldState* world_ref, const MonsterType& ref, const Position<double>& position) {
    switch (ref) {
        case MonsterType::BASIC:
            return BasicMonster(world_ref, position);
    }
}

Monster::Monster(WorldState* state, const int& health, const double& movement_speed_, const double& rotational_speed,
    const Position<double>& pos, const MonsterType& type) :
        world_ref_(state), health_(health), movement_speed_(movement_speed_), rotational_speed_(rotational_speed),
        pos_(pos), angle_(0), interface_(make_unique<MonsterInterface>(this)), type_(type),
        id_(Monster::instance_counter)
{
    Monster::instance_counter++;
}

Monster::~Monster() {
}

Monster::Monster(Monster&& other) :
        world_ref_(other.world_ref_), health_(other.health_), movement_speed_(other.movement_speed_),
        rotational_speed_(other.rotational_speed_), pos_(other.pos_), angle_(other.angle_),
        interface_(std::move(other.interface_)), type_(other.type_), id_(other.id_)
{
    interface_->reference_moved(this);
    other.interface_ = nullptr;
}

void Monster::set_position(const double& x, const double& y) {
    pos_.set_x(x);
    pos_.set_y(y);
}

const Position<double>& Monster::get_position() const {
    return pos_;
}

double& Monster::get_angle() {
    return angle_;
}

const double& Monster::get_rotational_speed() const {
    return rotational_speed_;
}

const double& Monster::get_movement_speed() const {
    return movement_speed_;
}

std::vector<MonsterEye> Monster::eyes() {
    // Implement cost function

    // Calculate the eyes values
    vector<MonsterEye> monster_eyes;
    const vector<double> eyes_direction = {M_PI / 3.0, 0.0, -M_PI / 3.0};
    for (double eye_direction : eyes_direction) {
        double distance = world_ref_->get_wall_distance(pos_, angle_ + eye_direction, 0.01);
        if (distance > 3.0) distance = 3.0;
        monster_eyes.push_back({ eye_direction, distance });
    }
    return monster_eyes;
}

void Monster::move(const MonsterMovement& movement) {
    requested_movements_.push_back(movement);
}

void Monster::rotate(const MonsterRotation& rotation) {
    requested_rotations_.push_back(rotation);
}

const std::vector<MonsterMovement>& Monster::get_requested_movements() const {
    return requested_movements_;
}

const std::vector<MonsterRotation>& Monster::get_requested_rotations() const {
    return requested_rotations_;
}

void Monster::clear_requests() {
    requested_movements_.clear();
    requested_rotations_.clear();
}

const double &Monster::get_angle() const {
    return angle_;
}

const int &Monster::get_health() const {
    return health_;
}

MonsterInterface* Monster::get_interface() {
    return interface_.get();
}

const MonsterType& Monster::get_type() const {
    return type_;
}

const unsigned int& Monster::get_identifier() const {
    return id_;
}

const int &Monster::bullet_struck(const int &bullet_damage) {
    health_ = health_ - bullet_damage <= 0 ? 0 : health_ - bullet_damage;
    return health_;
}
