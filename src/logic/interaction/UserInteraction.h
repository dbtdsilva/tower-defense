#ifndef TOWERDEFENSE_USERINTERACTION_H
#define TOWERDEFENSE_USERINTERACTION_H

#include <memory>
#include <vector>
#include "UserInteractionInterface.h"
#include "../helpers/Position.h"

class WorldState;

typedef struct {
    const TowerType type;
    const Position<int> position;
} TowerAddRequest;

class UserInteraction {
public:
    UserInteraction(WorldState*);

    const std::vector<TowerAddRequest>& get_tower_add_requests() const;
    UserInteractionInterface* get_user_interaction_interface();
    void clear_requests();
    void add_tower(const TowerType& type, const Position<int>& position);
private:
    std::unique_ptr<UserInteractionInterface> interface_;

    std::vector<TowerAddRequest> requests;
    WorldState* world_state_;
};


#endif //TOWERDEFENSE_USERINTERACTION_H
