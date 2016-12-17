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
    void clear_world_requests();
    void simulate_load(long load_ms);


    const std::vector<Position<double>> get_monsters_position(Position<double> tower_pos, double tower_range) const;
    const double get_wall_distance(const Position<double>&, const double&, const double&) const;
    UserInteractionInterface* get_user_interaction_interface();
    void serialize_data(std::ostream&) const;
    void delete_tower(unsigned int identifier);
    void delete_monster(unsigned int identifier);

    friend std::ostream& operator<<(std::ostream& os, const WorldState& obj);
private:
    void append_line_to_path(Position<int> src, Position<int> dst);

    const size_t width_, height_;
    std::vector<std::vector<PositionState>> map_;
    std::list<Position<int>> path_;
    Position<int> start_position, end_position;

    std::vector<std::unique_ptr<Tower>> towers_;
    std::vector<std::unique_ptr<Monster>> monsters_;
    std::vector<std::unique_ptr<Bullet>> bullets_;
    UserInteraction user_interaction_;

    unsigned int player_currency_;

    const unsigned int monsters_per_level_;
    unsigned int game_level_, monsters_left_to_spawn_, score_;
    unsigned int idle_cycles_, idle_cycles_before_spawn_;

    unsigned int lives_;

    int cycle_ms_;
};


#endif //RAZERNAGA_WORLDSTATE_H
