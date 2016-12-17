#ifndef TOWERDEFENSE_POSITION_H
#define TOWERDEFENSE_POSITION_H

#include <iostream>
#include <cereal/archives/binary.hpp>

template <class T>
class Position {
public:
    Position() {}
    Position(T x, T y) : x_(x), y_(y) {}
    Position(const Position& other) { *this = other; }
    ~Position() {}

    void set_x(const T& x) { this->x_ = x; }
    void set_y(const T& y) { this->y_ = y; }
    const T& get_x() const { return x_; }
    const T& get_y() const { return y_; }

    Position& operator=(const Position& other) {
        if (this != &other) {
            this->x_ = other.x_;
            this->y_ = other.y_;
        }
        return *this;
    }
    friend bool operator==(const Position& source, const Position& other) {
        return source.x_ == other.x_ && source.y_ == other.y_;
    }
    friend bool operator!=(const Position& source, const Position& other) {
        return !(source == other);
    }
    friend std::ostream& operator<< (std::ostream& os, const Position& other) {
        os << other.x_ << ", " << other.y_;
        return os;
    }
private:
    T x_, y_;

    friend class cereal::access;
    template<typename Archive>
    void serialize(Archive &archive)
    {
        archive(x_, y_);
    }
};

#endif //TOWERDEFENSE_POSITION_H
