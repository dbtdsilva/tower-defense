#ifndef TOWERDEFENSE_USERINTERACTIONAGENT_H
#define TOWERDEFENSE_USERINTERACTIONAGENT_H

class UserInteractionInterface;

class UserInteractionAgent {
public:
    UserInteractionAgent(UserInteractionInterface*);
    void take_action();
private:
    UserInteractionInterface* interface_;
};

#endif //TOWERDEFENSE_USERINTERACTIONAGENT_H
