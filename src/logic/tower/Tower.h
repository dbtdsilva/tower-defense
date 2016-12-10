#ifndef TOWERDEFENSE_TOWER_H
#define TOWERDEFENSE_TOWER_H

#include <memory>
#include <vector>
#include "../../helpers/Position.h"
#include "../../helpers/Definitions.h"
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
    const double& get_rotational_speed() const;
    const unsigned int& get_identifier() const;
    const double& get_angle() const;
    const int& get_damage() const;
    TowerInterface* get_interface();
    double& get_angle();

    void shoot();
    std::vector<Position<double>> radar();
    void rotate(const TowerRotation& rotation);
    void clear_requests();
protected:
    Tower(WorldState*, const int& damage, const int& radar_load_time, const int& cost, const int& range,
        const double& rotational_speed, const Position<int>& pos, const TowerType&);
private:
    static unsigned int instance_counter;

    const unsigned int id_;
    const int damage_, radar_load_time_, cost_, range_;
    const double rotational_speed_;
    const Position<int> pos_;
    const TowerType type_;

    double angle_;
    std::vector<double> requested_shoots_;
    std::vector<TowerRotation> requested_rotations_;
    std::unique_ptr<TowerInterface> interface_;
    WorldState* world_ref_;
};


#endif //TOWERDEFENSE_TOWERDATA_H
