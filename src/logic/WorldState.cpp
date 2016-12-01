//
// Created by myrddin on 28/11/16.
//

#include "WorldState.h"

WorldState::WorldState() {

}

bool WorldState::add_tower(Tower& tower) {
    if (player_currency < tower.get_cost())
        return false;

    player_currency -= tower.get_cost();
    towers_.push_back(tower);
    return true;
}

void WorldState::tower_shoot(const Bullet& bullet) {
    flying_bullets.push_back(bullet);
}