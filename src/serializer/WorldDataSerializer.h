#ifndef TOWERDEFENSE_WORLDDATASERIALIZER_H
#define TOWERDEFENSE_WORLDDATASERIALIZER_H

#include <vector>
#include "../helpers/Position.h"
#include "../helpers/Definitions.h"

typedef struct {
    Position<double> position;
    int damage;
} BulletData;

typedef struct {
    Position<double> position;
    MonsterType type;
    int health;
    double angle;
} MonsterData;

typedef struct {
    Position<double> position;
    TowerType type;
    double angle;
} TowerData;

typedef struct {
    std::vector<TowerData> towers;
    std::vector<MonsterData> monsters;
    std::vector<BulletData> bullets;
} WorldData;

#endif //TOWERDEFENSE_WORLDDATASERIALIZER_H
