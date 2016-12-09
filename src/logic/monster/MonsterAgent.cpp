#include "MonsterAgent.h"
#include "MonsterInterface.h"

MonsterAgent::MonsterAgent(MonsterInterface * ref) : monster_ref_(ref) {
}

void MonsterAgent::take_action() {
    monster_ref_->rotate();
    monster_ref_->right_eye();
    // ...
}
