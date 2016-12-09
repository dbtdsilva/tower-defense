#ifndef TOWERDEFENSE_MONSTERAGENT_H
#define TOWERDEFENSE_MONSTERAGENT_H

class MonsterInterface;

class MonsterAgent {
public:
    MonsterAgent(MonsterInterface*);

    void take_action();
private:
    MonsterInterface* monster_ref_;
};

#endif //TOWERDEFENSE_MONSTERAGENT_H
