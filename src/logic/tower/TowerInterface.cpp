#include "TowerInterface.h"

using namespace std;

TowerInterface::TowerInterface(Tower* reference) : tower_reference_(reference) {

}

void TowerInterface::shoot() {
    tower_reference_->shoot();
}

void TowerInterface::radar() {
    tower_reference_->radar();
}

void TowerInterface::rotate() {
    tower_reference_->rotate();
}