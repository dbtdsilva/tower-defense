#ifndef TOWERDEFENSE_TOWER_H
#define TOWERDEFENSE_TOWER_H

#include <tuple>
#include <memory>
#include "../monster/Monster.h"
#include "../helpers/Position.h"
#include "../WorldState.h"

enum TowerType { SIMPLE, COMPLEX };
enum TowerDirection {LEFT, RIGHT};

class WorldState;
class Monster;

class Tower {
    public:
        static Tower create_tower(WorldState& world_ref, const TowerType& ref, const Position<double>& position);
        void shoot();
        std::vector<Monster> radar();

        const int& get_cost() const;
    protected:
        Tower(WorldState& state, const int& damage, const int& radar_load_time, const int& cost, const int& range,
            const double& rotational_speed, const Position<double>& pos);
    private:
        const int damage_, radar_load_time_, cost_, range_;
        const double rotational_speed_;
        const Position<double> pos_;

        double angle_;
        WorldState& world_ref_;
};


#endif //TOWERDEFENSE_TOWER_H
