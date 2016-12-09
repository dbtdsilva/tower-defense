#ifndef TOWERDEFENSE_MONSTER_H
#define TOWERDEFENSE_MONSTER_H

#include <memory>
#include "../helpers/Position.h"
#include "../helpers/Definitions.h"
#include "MonsterInterface.h"
#include "MonsterAgent.h"

class WorldState;

class Monster {
public:
    static Monster add_monster(WorldState* world_ref, const MonsterType& ref, const Position<double>& position);

    const Position<double>& get_position() const;
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
    std::unique_ptr<MonsterInterface> interface_;
    WorldState* world_ref_;
};


#endif //TOWERDEFENSE_MONSTER_H
