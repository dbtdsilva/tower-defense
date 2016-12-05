#ifndef TOWERDEFENSE_TOWERDATA_H
#define TOWERDEFENSE_TOWERDATA_H

#include "../WorldState.h"
#include "../helpers/Position.h"

enum TowerType { SIMPLE, COMPLEX };

class WorldState;

class TowerData {
public:
    static TowerData create_tower(WorldState& world_ref, const TowerType& ref, const Position<double>& position);
    const int& get_cost() const;
protected:
    TowerData(WorldState&, const int& damage, const int& radar_load_time, const int& cost, const int& range,
    const double& rotational_speed, const Position<double>& pos);
private:
    const int damage_, radar_load_time_, cost_, range_;
    const double rotational_speed_;
    const Position<double> pos_;

    double angle_;
    WorldState& world_ref_;
};


#endif //TOWERDEFENSE_TOWERDATA_H
