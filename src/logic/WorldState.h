#ifndef RAZERNAGA_WORLDSTATE_H
#define RAZERNAGA_WORLDSTATE_H

#include <vector>
#include <list>
#include "tower/Tower.h"
#include "monster/Monster.h"
#include "helpers/Position.h"
#include "interaction/UserInteraction.h"

class Monster;

enum PositionState { EMPTY, PATH, TOWER };

typedef struct {
    Position<double> position;
    const double angle;
    const double speed;
} Bullet;

class WorldState {
public:
    WorldState(size_t width, size_t height);
    void update_world_state();

    const std::vector<Monster>& get_monsters() const;
    const double get_wall_distance(const Position<double>&, const double&, const double&) const;
    UserInteractionInterface* get_user_interaction_interface();

    friend std::ostream& operator<<(std::ostream& os, const WorldState& obj);
private:
    void append_line_to_path(Position<int> src, Position<int> dst);

    const size_t width_, height_;
    const int units_per_cell;
    std::vector<std::vector<PositionState>> map_;
    std::list<Position<int>> path_;

    std::vector<Tower> towers_;
    std::vector<Monster> monsters_;
    std::vector<Bullet> bullets_;
    UserInteraction user_interaction_;

    unsigned int simulation_time_;
    unsigned int player_currency;
};


#endif //RAZERNAGA_WORLDSTATE_H
