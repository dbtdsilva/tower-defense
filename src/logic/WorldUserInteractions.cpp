#include "WorldUserInteractions.h"

using namespace std;

WorldUserInteractions::WorldUserInteractions(WorldState& ref) : world_ref_(ref) {
}

bool WorldUserInteractions::add_tower(const TowerType& type, const Position<double>& pos) {
    Tower requested_tower = Tower::create_tower(world_ref_, type, pos);
    return world_ref_.add_tower(requested_tower);
}