#ifndef TOWERDEFENSE_EYE_H
#define TOWERDEFENSE_EYE_H

class MonsterEye {
public:
    MonsterEye() : wall_distance(0), direction(0) {}
    MonsterEye(double direction, double wall_distance) : direction(direction), wall_distance(wall_distance) {}

    const double direction;
    double wall_distance;
};

#endif //TOWERDEFENSE_EYE_H
