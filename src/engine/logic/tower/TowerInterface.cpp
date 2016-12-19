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

const std::vector<Position<double>> TowerInterface::radar() {
    return tower_reference_->radar();
}

void TowerInterface::rotate(const TowerRotation& rotation) {
    tower_reference_->rotate(rotation);
}

const Position<int> TowerInterface::get_position() const {
    return tower_reference_->get_position();
}

const double TowerInterface::get_angle() const {
    return tower_reference_->get_angle();
}

const unsigned int TowerInterface::get_identifier() const {
    return tower_reference_->get_identifier();
}