#ifndef TOWERDEFENSE_USERINTERACTION_H
#define TOWERDEFENSE_USERINTERACTION_H

#include <memory>
#include <vector>
#include <helpers/Position.h>
#include "UserInteractionInterface.h"

class WorldState;

typedef struct GameStatusRequest {
    GameStatusRequest(const GameStatus& status) :
            status(status) {};
    GameStatusRequest(const GameStatusRequest& other) : status(other.status) {}
    GameStatusRequest& operator=(const GameStatusRequest& other) {
        if (this != &other)
            this->status = status;

        return *this;
    }

    GameStatus status;
} GameStatusRequest;

typedef struct TowerRequest {
    TowerRequest(const TowerType& type, const Position<int> position, const TowerOperation& operation) :
            type(type), position(position), operation(operation) {}
    TowerRequest(const TowerRequest& other) : type(other.type), position(other.position), operation(other.operation) {}
    TowerRequest& operator=(const TowerRequest& other) {
        if (this != &other) {
            this->type = type;
            this->position = position;
            this->operation = operation;
        }
        return *this;
    }
    TowerType type;
    Position<int> position;
    TowerOperation operation;
} TowerRequest;

class UserInteraction {
public:
    UserInteraction(WorldState*);

    const std::vector<TowerRequest>& get_tower_requests() const;
    UserInteractionInterface* get_user_interaction_interface();
    void clear_requests();
    void add_tower(const TowerType& type, const Position<int>& position);
    void remove_tower(const Position<int>& position);
    GameStatusRequest* get_game_status_request() const;
    void play_game();
    void pause_game();
private:
    std::unique_ptr<UserInteractionInterface> interface_;

    std::vector<TowerRequest> requests;
    std::unique_ptr<GameStatusRequest> gameStatusRequest;
    WorldState* world_state_;
};


#endif //TOWERDEFENSE_USERINTERACTION_H
