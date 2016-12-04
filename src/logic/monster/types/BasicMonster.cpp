#include "BasicMonster.h"

BasicMonster::BasicMonster(WorldState& world_ref, const Position<double>& pos) :
    Monster(world_ref, 100, 1000, 1000, pos)
{
}