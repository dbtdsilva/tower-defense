#ifndef RAZERNAGA_WORLDSTATE_H
#define RAZERNAGA_WORLDSTATE_H

#include <vector>
#include "tower/TowerData.h"
#include "monster/Monster.h"
#include "tower/Bullet.h"

class Tower;
class Monster;
class Bullet;

class WorldState {
public:
    WorldState();

    /*bool add_tower(Tower& tower);
    void tower_shoot(const Bullet& bullet);
    std::vector<Monster> tower_radar();
*/
    void update_world_state();
private:
    std::vector<TowerData> towers_;
    std::vector<Monster> monsters_;
    std::vector<Bullet> flying_bullets;

    unsigned int simulation_time_;
    unsigned int player_currency;
};


#endif //RAZERNAGA_WORLDSTATE_H
