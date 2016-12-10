#ifndef TOWERDEFENSE_WORLDDATASERIALIZER_H
#define TOWERDEFENSE_WORLDDATASERIALIZER_H

#include <cereal/archives/binary.hpp>
#include <vector>
#include "../helpers/Position.h"
#include "../helpers/Definitions.h"

class BulletData {
public:
    BulletData() : position_(0,0), damage_(0) {}
    BulletData(const Position<double>& pos, const int& damage) : position_(pos), damage_(damage) {}

    Position<double> position_;
    int damage_;
private:
    friend class cereal::access;
    template<typename Archive>
    void serialize(Archive &archive) {
        archive(position_, damage_);
    }
};

class MonsterData {
public:
    MonsterData() : position_(0,0), type_(MonsterType::BASIC), health_(0), angle_(0) {}
    MonsterData(const Position<double>& position, const MonsterType& type, const int& health, const double& angle) :
        position_(position), type_(type), health_(health), angle_(angle) {}

    Position<double> position_;
    MonsterType type_;
    int health_;
    double angle_;
private:
    friend class cereal::access;
    template<typename Archive>
    void serialize(Archive &archive) {
        archive(position_, type_, health_, angle_);
    }
};

class TowerData {
public:
    TowerData() : position_(0,0), type_(TowerType::SIMPLE), angle_(0) {}
    TowerData(const Position<double>& position, const TowerType& type, const double& angle) :
            position_(position), type_(type), angle_(angle) {}

    Position<double> position_;
    TowerType type_;
    double angle_;
private:
    friend class cereal::access;
    template<typename Archive>
    void serialize(Archive &archive) {
        archive(position_, type_, angle_);
    }
};

class WorldData {
public:
    WorldData() {}

    std::vector<TowerData> towers_;
    std::vector<MonsterData> monsters_;
    std::vector<BulletData> bullets_;
private:
    friend class cereal::access;
    template<typename Archive>
    void serialize(Archive &archive) {
        archive(bullets_, towers_, monsters_);
    }
};

#endif //TOWERDEFENSE_WORLDDATASERIALIZER_H
