#ifndef TOWERDEFENSE_MONSTER_H
#define TOWERDEFENSE_MONSTER_H

#include "../helpers/Position.h"
#include "../WorldState.h"
#include "MonsterAgent.h"

enum MonsterType { BASIC, INSANE };

class WorldState;
class MonsterAgent;

class Monster {
public:
    static Monster add_monster(WorldState* world_ref, const MonsterType& ref, const Position<double>& position);

    void move();
    void left_eye();
    void middle_eye();
    void right_eye();
    void rotate();
protected:
    Monster(WorldState* state, const int& health, const double& movement_speed_, const double& rotational_speed,
            const Position<double>& pos);
private:
    const int health_;
    const double rotational_speed_, movement_speed_;

    Position<double> pos_;
    double angle_;

    std::unique_ptr<MonsterAgent> agent_;
    WorldState* world_ref_;
};


#endif //TOWERDEFENSE_MONSTER_H
