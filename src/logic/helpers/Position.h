#ifndef TOWERDEFENSE_POSITION_H
#define TOWERDEFENSE_POSITION_H

#include <iostream>

template <class T>
class Position {
public:
    Position(T x, T y) : x_(x), y_(y) {}
    Position(const Position& other) { *this = other; }
    ~Position() {}

    void set_x(const T& x) { this->x_ = x; }
    void set_y(const T& y) { this->y_ = y; }
    T& get_x() { return x_; }
    T& get_y() { return y_; }

    Position& operator=(const Position& other) {
        if (this != &other) {
            this->x_ = other.x_;
            this->y_ = other.y_;
        }
        return *this;
    }
    bool operator==(const Position& other) {
        return this->x_ == other.x_ && this->y_ == other.y_;
    }
    bool operator!=(const Position& other) {
        return !(*this == other);
    }
    friend std::ostream& operator<< (std::ostream& os, const Position& other) {
        os << other.x_ << ", " << other.y_;
        return os;
    }
private:
    T x_, y_;
};

#endif //TOWERDEFENSE_POSITION_H
