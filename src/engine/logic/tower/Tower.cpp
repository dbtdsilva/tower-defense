#include "Tower.h"

#include <cmath>
#include "types/SimpleTower.h"
#include "types/ComplexTower.h"
#include "../monster/Monster.h"
#include "../WorldState.h"

using namespace std;

unsigned int Tower::instance_counter = 0;

Tower Tower::create_tower(WorldState* world_ref, const TowerType& ref, const Position<int>& position) {
    switch (ref) {
        case TowerType::SIMPLE:
            return SimpleTower(world_ref, position);
        case TowerType::COMPLEX:
            return ComplexTower(world_ref, position);
    }
}

Tower::~Tower() {
    // Only smart pointers are used
}

Tower::Tower(WorldState* state, const int& damage, const int& radar_load_time, const int& cost, const double& range,
             const double& rotational_speed, const Position<int>& pos, const TowerType& type) :
        damage_(damage), radar_load_time_(radar_load_time), cost_(cost), range_(range),
        rotational_speed_(rotational_speed), pos_(pos), angle_(0), world_ref_(state),
        interface_(make_unique<TowerInterface>(this)), id_(Tower::instance_counter), type_(type)
{
    Tower::instance_counter++;
}

Tower::Tower(Tower&& other)  :
        cost_(other.cost_), pos_(other.pos_), range_(other.range_), rotational_speed_(other.rotational_speed_),
        damage_(other.damage_), radar_load_time_(other.radar_load_time_), angle_(other.angle_), id_(other.id_),
        interface_(std::move(other.interface_)), world_ref_(other.world_ref_), type_(other.type_)
{
    interface_->reference_moved(this);
    other.interface_ = nullptr;
}

const int& Tower::get_cost() const {
    return cost_;
}

const Position<int>& Tower::get_position() const {
    return pos_;
}

const std::vector<TowerRotation>& Tower::get_requested_rotations() const {
    return requested_rotations_;
}

const std::vector<double>& Tower::get_requested_shoots() const {
    return requested_shoots_;
}

const double& Tower::get_rotational_speed() const {
    return rotational_speed_;
}

const double &Tower::get_angle() const {
    return angle_;
}

double& Tower::get_angle() {
    return angle_;
}

const unsigned int& Tower::get_identifier() const {
    return id_;
}

TowerInterface* Tower::get_interface() {
    return interface_.get();
}

void Tower::clear_requests() {
    requested_rotations_.clear();
    requested_shoots_.clear();
}

void Tower::shoot() {
    requested_shoots_.push_back(angle_);
}

const vector<Position<double>> Tower::radar() const {
    // Implement a cost function
    world_ref_->simulate_load(radar_load_time_);

    // Return the value
    vector<Position<double>> monsters_in_range;
    return world_ref_->get_monsters_position(Position<double>(pos_.get_x() + 0.5, pos_.get_y() + 0.5), range_ + 0.5);
}

void Tower::rotate(const TowerRotation& rotation) {
    requested_rotations_.push_back(rotation);
}

const TowerType &Tower::get_type() const {
    return type_;
}

const int &Tower::get_damage() const {
    return damage_;
}

const double& Tower::get_range() const {
    return range_;
}
