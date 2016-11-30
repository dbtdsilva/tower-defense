//
// Created by myrddin on 30/11/16.
//

#include "TowerAgent.h"

TowerAgent::TowerAgent(Tower& ref) : tower_ref_(ref) {

}

void TowerAgent::take_action() {
    tower_ref_.shoot();
}