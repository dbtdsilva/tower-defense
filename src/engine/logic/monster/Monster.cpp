#include "Monster.h"

#include <cmath>
#include "types/BasicMonster.h"
#include "types/InsaneMonster.h"

using namespace std;

unsigned int Monster::instance_counter = 0;

Monster Monster::add_monster(WorldState* world_ref, const MonsterType& ref, const Position<double>& position) {
    switch (ref) {
        case MonsterType::BASIC:
            return BasicMonster(world_ref, position);
        case MonsterType::INSANE:
            return InsaneMonster(world_ref, position);
    }
}

Monster::Monster(WorldState* state, const int& health, const double& movement_speed, const double& rotational_speed,
                 const RTIME& eyes_cost_ms, const Position<double>& pos, const MonsterType& type) :
        world_ref_(state), health_(health), movement_speed_(movement_speed), rotational_speed_(rotational_speed),
        pos_(pos), angle_(0), interface_(make_unique<MonsterInterface>(this)), type_(type), eyes_cost_ms_(eyes_cost_ms),
        id_(Monster::instance_counter)
{
    const vector<double> eyes_direction = {M_PI / 3.0, 0.0, -M_PI / 3.0};
    for (double eye_direction : eyes_direction) {
        monster_eyes_.push_back(MonsterEye(eye_direction, 0.0));
    }
    Monster::instance_counter++;
}

Monster::~Monster() {
}

Monster::Monster(Monster&& other) :
        world_ref_(other.world_ref_), health_(other.health_), movement_speed_(other.movement_speed_),
        rotational_speed_(other.rotational_speed_), pos_(other.pos_), angle_(other.angle_),
        eyes_cost_ms_(other.eyes_cost_ms_), interface_(std::move(other.interface_)), type_(other.type_),
        id_(other.id_),  monster_eyes_(other.monster_eyes_)
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

std::vector<MonsterEye>& Monster::eyes() {
    // Implement cost function
    world_ref_->simulate_load(eyes_cost_ms_);
    // Calculate the eyes values
    return monster_eyes_;
}

void Monster::update_eyes() {
    for (MonsterEye& eye : monster_eyes_) {
        double distance = world_ref_->get_wall_distance(pos_, angle_ + eye.direction, 0.01);
        if (distance > 3.0) distance = 3.0;
        eye.wall_distance = distance;
    }
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
    health_ = bullet_damage >= health_ ? 0 : health_ - bullet_damage;
    return health_;
}
