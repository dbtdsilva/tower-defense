#ifndef TOWERDEFENSE_TOWER_H
#define TOWERDEFENSE_TOWER_H

#include <tuple>
#include "Position.h"

class Tower {
    public:
        Tower(const int& damage, const int& radar_cooldown_time, const int& cost, const int& range,
              const double& rotational_speed, const Position& pos);
        void shoot();

    private:
        const int damage_, radar_cooldown_time_, cost_, range_;
        const double rotational_speed_;
        const Position pos_;

        double angle_;

};


#endif //TOWERDEFENSE_TOWER_H
