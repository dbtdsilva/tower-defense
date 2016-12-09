#ifndef TOWERDEFENSE_MONSTER_H
#define TOWERDEFENSE_MONSTER_H

#include <memory>
#include <vector>
#include "../helpers/Position.h"
#include "../helpers/Definitions.h"
#include "MonsterEye.h"
#include "MonsterInterface.h"

class WorldState;

class Monster {
public:
    static Monster add_monster(WorldState* world_ref, const MonsterType& ref, const Position<double>& position);
    ~Monster();
    // No copies or assignments are allowed (only moves)
    Monster(const Monster&) = delete;
    Monster& operator=(const Monster& other) = delete;
    Monster& operator=(Monster&& other) = delete;
    Monster(Monster&&);

    const Position<double>& get_position() const;
    std::vector<MonsterEye> eyes();
    void move(const MonsterMovement&);
    void rotate(const MonsterRotation&);
protected:
    Monster(WorldState* state, const int& health, const double& movement_speed_, const double& rotational_speed,
            const Position<double>& pos);
private:
    const int health_;
    const double rotational_speed_, movement_speed_;

    Position<double> pos_;
    double angle_;

    std::unique_ptr<MonsterInterface> interface_;
    WorldState* world_ref_;
};


#endif //TOWERDEFENSE_MONSTER_H
