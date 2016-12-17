#ifndef TOWERDEFENSE_VIEWERDATASERIALIZER_H
#define TOWERDEFENSE_VIEWERDATASERIALIZER_H

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <vector>
#include "helpers/Position.h"
#include "helpers/Definitions.h"

struct ViewerData {
    virtual ViewerRequest get_type() = 0;
};

struct GameStatusData : public ViewerData {
    GameStatusData() : status_(GameStatus::PLAY) {}
    GameStatusData(GameStatus status) : status_(status) {}

    ViewerRequest get_type() { return ViewerRequest::GAME_STATUS; }

    GameStatus status_;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(status_);
    }
};

struct OperationTowerData : public ViewerData {
    OperationTowerData() : operation_(TowerOperation::INSERT), position_(0,0), type_(TowerType::SIMPLE) {}
    OperationTowerData(const TowerOperation& operation, const Position<int>& position, const TowerType& type) :
            operation_(operation), position_(position), type_(type) {}

    ViewerRequest get_type() { return ViewerRequest::TOWER; }

    TowerOperation operation_;
    Position<int> position_;
    TowerType type_;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(operation_, position_, type_);
    }
};

CEREAL_REGISTER_TYPE(GameStatusData);
CEREAL_REGISTER_TYPE(OperationTowerData);

CEREAL_REGISTER_POLYMORPHIC_RELATION(ViewerData, GameStatusData);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ViewerData, OperationTowerData);

#endif //TOWERDEFENSE_VIEWERDATASERIALIZER_H
