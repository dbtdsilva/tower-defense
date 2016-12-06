#include "MonsterAgent.h"

MonsterAgent::MonsterAgent(std::unique_ptr<MonsterInterface> ref) : monster_ref_(std::move(ref)) {
}

void MonsterAgent::take_action() {
    monster_ref_->rotate();
    monster_ref_->right_eye();
    // ...
}
