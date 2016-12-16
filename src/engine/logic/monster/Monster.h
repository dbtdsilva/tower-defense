#ifndef TOWERDEFENSE_MONSTER_H
#define TOWERDEFENSE_MONSTER_H

#include <memory>
#include <vector>
#include <helpers/Position.h>
#include <helpers/Definitions.h>
#include "MonsterEye.h"
#include "MonsterInterface.h"

class WorldState;

class Monster {
public:
    static Monster add_monster(WorldState* world_ref, const MonsterType& ref, const Position<double>& position);
    ~Monster();
    // No copies or assignments are allowed (only moves)
    Monster(Monster&&);
    Monster(const Monster&) = delete;
    Monster& operator=(const Monster& other) = delete;
    Monster& operator=(Monster&& other) = delete;

    std::vector<MonsterEye> eyes();
    void move(const MonsterMovement&);
    void rotate(const MonsterRotation&);

    void set_position(const double& x, const double& y);
    const Position<double>& get_position() const;
    double& get_angle();
    const double& get_angle() const;
    const int& get_health() const;
    const MonsterType& get_type() const;
    const double& get_rotational_speed() const;
    const double& get_movement_speed() const;
    const std::vector<MonsterMovement>& get_requested_movements() const;
    const std::vector<MonsterRotation>& get_requested_rotations() const;
    const unsigned int& get_identifier() const;
    const int& bullet_struck(const int& bullet_damage);
    MonsterInterface* get_interface();
    void clear_requests();
protected:
    Monster(WorldState* state, const int& health, const double& movement_speed_, const double& rotational_speed,
            const Position<double>& pos, const MonsterType& type);
private:
    static unsigned int instance_counter;

    const unsigned int id_;
    const double rotational_speed_, movement_speed_;
    const MonsterType type_;

    int health_;
    Position<double> pos_;
    double angle_;

    std::vector<MonsterMovement> requested_movements_;
    std::vector<MonsterRotation> requested_rotations_;
    std::unique_ptr<MonsterInterface> interface_;
    WorldState* world_ref_;
};


#endif //TOWERDEFENSE_MONSTER_H
