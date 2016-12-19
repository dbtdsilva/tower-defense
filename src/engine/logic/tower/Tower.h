#ifndef TOWERDEFENSE_TOWER_H
#define TOWERDEFENSE_TOWER_H

#include <memory>
#include <vector>
#include <helpers/Position.h>
#include <helpers/Definitions.h>
#include <native/types.h>
#include <random>
#include "TowerInterface.h"

class WorldState;

class Tower {
public:
    static Tower create_tower(WorldState* world_ref, const TowerType& ref, const Position<int>& position);
    ~Tower();
    // No copies or assignments are allowed (only moves)
    Tower(const Tower&) = delete;
    Tower& operator=(const Tower& other) = delete;
    Tower& operator=(Tower&& other) = delete;
    Tower(Tower&&);

    const int& get_cost() const;
    const std::vector<TowerRotation>& get_requested_rotations() const;
    const std::vector<double>& get_requested_shoots() const;
    const Position<int>& get_position() const;
    const TowerType& get_type() const;
    const double& get_range() const;
    const double& get_rotational_speed() const;
    const unsigned int& get_identifier() const;
    const double& get_angle() const;
    const int& get_damage() const;
    TowerInterface* get_interface();
    double& get_angle();
    bool able_to_shoot();

    void shoot();
    const std::vector<Position<double>> radar();
    void rotate(const TowerRotation& rotation);
    void clear_requests();
protected:
    Tower(WorldState*, const int& damage, const int& weapon_load_cycles, const RTIME& radar_load_time,
          const int& cost, const double& range, const double& rotational_speed, const Position<int>& pos,
          const TowerType&);
private:
    static unsigned int instance_counter;

    const RTIME radar_load_time_;
    const unsigned int id_;
    const int damage_, cost_, weapon_load_cycles_;
    const double rotational_speed_, range_;
    const Position<int> pos_;
    const TowerType type_;

    double angle_;
    unsigned int internal_cycle_between_shots_;
    std::vector<double> requested_shoots_;
    std::vector<TowerRotation> requested_rotations_;
    std::unique_ptr<TowerInterface> interface_;
    WorldState* world_ref_;

    std::normal_distribution<double> distribution_;
    std::default_random_engine generator_;
};


#endif //TOWERDEFENSE_TOWERDATA_H
