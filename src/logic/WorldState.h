#ifndef RAZERNAGA_WORLDSTATE_H
#define RAZERNAGA_WORLDSTATE_H

#include <vector>
#include "tower/Tower.h"
#include "monster/Monster.h"

class WorldState {
public:

private:
    std::vector<Tower> towers_;
    std::vector<Monster> monsters_;
    unsigned int simulation_time_;
};


#endif //RAZERNAGA_WORLDSTATE_H
