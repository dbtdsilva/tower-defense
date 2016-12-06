#ifndef RAZERNAGA_WORLDSTATE_H
#define RAZERNAGA_WORLDSTATE_H

#include <vector>
#include "tower/Tower.h"
#include "monster/Monster.h"

class Tower;
class Monster;
class Bullet;

class WorldState {
public:
    WorldState();
    void update_world_state();
private:
    std::vector<Tower> towers_;
    std::vector<Monster> monsters_;

    unsigned int simulation_time_;
    unsigned int player_currency;
};


#endif //RAZERNAGA_WORLDSTATE_H
