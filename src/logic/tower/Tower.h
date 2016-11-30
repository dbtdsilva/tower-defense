#ifndef TOWERDEFENSE_TOWER_H
#define TOWERDEFENSE_TOWER_H

#include <tuple>
#include <memory>
#include "../helpers/Position.h"

enum TowerType { SIMPLE, COMPLEX };

class Tower {
    public:
        static std::unique_ptr<Tower> create_tower(const TowerType& ref, const Position<double>& position);
        void shoot();

    protected:
        Tower(const int& damage, const int& radar_load_time, const int& cost, const int& range,
            const double& rotational_speed, const Position<double>& pos);
    private:
        const int damage_, radar_load_time_, cost_, range_;
        const double rotational_speed_;
        const Position<double> pos_;

        double angle_;
};


#endif //TOWERDEFENSE_TOWER_H
