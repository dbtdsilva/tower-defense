#include "SimpleTower.h"

SimpleTower::SimpleTower(WorldState& world_ref, const Position<double>& pos) :
        Tower(world_ref, 100, 1, 100, 200, 10, pos) {
}