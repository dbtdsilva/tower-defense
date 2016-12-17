#ifndef TOWERDEFENSE_USERINTERACTION_H
#define TOWERDEFENSE_USERINTERACTION_H

#include <memory>
#include <vector>
#include <helpers/Position.h>
#include "UserInteractionInterface.h"

class WorldState;

typedef struct {
    const TowerType type;
    const Position<int> position;
    const TowerOperation operation;
} TowerRequest;

class UserInteraction {
public:
    UserInteraction(WorldState*);

    const std::vector<TowerRequest>& get_tower_add_requests() const;
    UserInteractionInterface* get_user_interaction_interface();
    void clear_requests();
    void add_tower(const TowerType& type, const Position<int>& position);
    void remove_tower(const Position<int>& position);
private:
    std::unique_ptr<UserInteractionInterface> interface_;

    std::vector<TowerRequest> requests;
    WorldState* world_state_;
};


#endif //TOWERDEFENSE_USERINTERACTION_H
