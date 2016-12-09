#ifndef TOWERDEFENSE_USERINTERACTION_H
#define TOWERDEFENSE_USERINTERACTION_H

#include <memory>
#include <vector>
#include "UserInteractionInterface.h"
#include "UserInteractionAgent.h"
#include "../helpers/Position.h"

class WorldState;

typedef struct {
    const TowerType type;
    const Position<double> position;
} TowerAddRequest;

class UserInteraction {
public:
    UserInteraction(WorldState*);

    const std::vector<TowerAddRequest>& get_tower_add_requests() const;
    UserInteractionAgent* get_user_interaction_agent();
    void clear_requests();
    void add_tower(const TowerType& type, const Position<double>& position);
private:
    std::unique_ptr<UserInteractionInterface> interface_;
    std::unique_ptr<UserInteractionAgent> agent_;

    std::vector<TowerAddRequest> requests;
    WorldState* world_state_;
};


#endif //TOWERDEFENSE_USERINTERACTION_H
