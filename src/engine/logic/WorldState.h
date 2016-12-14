#ifndef RAZERNAGA_WORLDSTATE_H
#define RAZERNAGA_WORLDSTATE_H

#include <vector>
#include <list>
#include <helpers/Position.h>
#include "tower/Tower.h"
#include "tower/Bullet.h"
#include "monster/Monster.h"
#include "interaction/UserInteraction.h"

class Monster;

class WorldState {
public:
    WorldState(size_t width, size_t height, int god_task_period_ms);

    std::vector<EntityModification> update_world_state();

    const std::vector<Monster>& get_monsters() const;
    const double get_wall_distance(const Position<double>&, const double&, const double&) const;
    UserInteractionInterface* get_user_interaction_interface();
    void serialize_data(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& os, const WorldState& obj);
private:
    void append_line_to_path(Position<int> src, Position<int> dst);

    const size_t width_, height_;
    std::vector<std::vector<PositionState>> map_;
    std::list<Position<int>> path_;
    Position<double> start_position, end_position;

    std::vector<Tower> towers_;
    std::vector<Monster> monsters_;
    std::vector<Bullet> bullets_;
    UserInteraction user_interaction_;

    unsigned int simulation_time_;
    unsigned int player_currency_;

    const unsigned int monsters_per_level_;
    unsigned int game_level_;
    unsigned int monsters_left_to_spawn_;
    unsigned int idle_cycles_;
    unsigned int idle_cycles_before_spawn_;

    int cycle_ms_;
};


#endif //RAZERNAGA_WORLDSTATE_H
