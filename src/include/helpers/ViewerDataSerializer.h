#ifndef TOWERDEFENSE_VIEWERDATASERIALIZER_H
#define TOWERDEFENSE_VIEWERDATASERIALIZER_H

#include <cereal/archives/binary.hpp>
#include <vector>
#include "helpers/Position.h"
#include "helpers/Definitions.h"

class OperationTowerData {
public:
    OperationTowerData() : operation_(TowerOperation::INSERT), position_(0,0), type_(TowerType::SIMPLE) {}
    OperationTowerData(const TowerOperation& operation, const Position<int>& position, const TowerType& type) :
            operation_(operation), position_(position), type_(type) {}

    TowerOperation operation_;
    Position<int> position_;
    TowerType type_;
private:
    friend class cereal::access;
    template<typename Archive>
    void serialize(Archive &archive) {
        archive(operation_, position_, type_);
    }
};

union ViewerRequestData {
    ViewerRequestData() : status_(GameStatus::PLAY) {}
    ~ViewerRequestData() {}

    GameStatus status_;
    OperationTowerData tower_;
};

class ViewerData {
public:
    ViewerData() : type(ViewerRequest::GAME_STATUS) {}
    ~ViewerData() {}

    ViewerRequest type;
    ViewerRequestData data;
private:

    friend class cereal::access;
    template<typename Archive>
    void serialize(Archive &archive) {
        switch (type) {
            case ViewerRequest::GAME_STATUS:
                archive(type, data.status_);
                break;
            case ViewerRequest::TOWER:
                archive(type, data.tower_);
                break;
        }
    }
};

#endif //TOWERDEFENSE_VIEWERDATASERIALIZER_H
