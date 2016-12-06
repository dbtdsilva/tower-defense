#ifndef TOWERDEFENSE_USERINTERACTION_H
#define TOWERDEFENSE_USERINTERACTION_H

#include <memory>
#include "UserInteractionInterface.h"

class UserInteraction {
public:
    UserInteraction(std::unique_ptr<UserInteractionInterface>);

    void take_action();
private:
    std::unique_ptr<UserInteractionInterface> interface_;
};


#endif //TOWERDEFENSE_USERINTERACTION_H
