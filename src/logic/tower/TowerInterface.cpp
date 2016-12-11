#include "TowerInterface.h"
#include "Tower.h"

using namespace std;

TowerInterface::TowerInterface(Tower* reference) : tower_reference_(reference) {

}

void TowerInterface::reference_moved(Tower* reference) {
    tower_reference_ = reference;
}

void TowerInterface::shoot() {
    tower_reference_->shoot();
}

void TowerInterface::radar() {
    tower_reference_->radar();
}

void TowerInterface::rotate(const TowerRotation& rotation) {
    tower_reference_->rotate(rotation);
}