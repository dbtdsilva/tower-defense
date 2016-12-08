#ifndef RAZERNAGA_WORLDSTATE_H
#define RAZERNAGA_WORLDSTATE_H

#include <vector>
#include <list>
#include "tower/Tower.h"
#include "monster/Monster.h"
#include "helpers/Position.h"

class Tower;
class Monster;
class Bullet;

enum PositionState { EMPTY, PATH, TOWER };

class WorldState {
public:
    WorldState(size_t width, size_t height);
    void update_world_state();

    friend std::ostream& operator<<(std::ostream& os, const WorldState& obj);
private:
    void append_line_to_path(Position<int> src, Position<int> dst);

    const size_t width_, height_;
    std::vector<std::vector<PositionState>> map_;
    std::list<Position<int>> path_;


    std::vector<Tower> towers_;
    std::vector<Monster> monsters_;

    unsigned int simulation_time_;
    unsigned int player_currency;
};


#endif //RAZERNAGA_WORLDSTATE_H
