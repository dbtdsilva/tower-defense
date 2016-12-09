#include "WorldState.h"
#include <iostream>

using namespace std;

WorldState::WorldState(size_t width, size_t height) : width_(width), height_(height),
                                                      map_(width, std::vector<PositionState>(height, EMPTY)),
                                                      units_per_cell(500)
{
    append_line_to_path(Position<int>(0, 1), Position<int>(width_ - 2, 1));
    append_line_to_path(Position<int>(width_ - 2, 1), Position<int>(width_ - 2, height_ - 2));
    append_line_to_path(Position<int>(width_ - 2, height_ - 2), Position<int>(3, height_ - 2));
    append_line_to_path(Position<int>(3, height_ - 2), Position<int>(3, height_ - 4));
    append_line_to_path(Position<int>(3, height_ - 4), Position<int>(width_ - 4, height_ - 4));
    append_line_to_path(Position<int>(width_ - 4, height_ - 4), Position<int>(width_ - 4, 3));
    append_line_to_path(Position<int>(width_ - 4, 3), Position<int>(1, 3));
    append_line_to_path(Position<int>(1, 3), Position<int>(1, height_ - 1));
}

void WorldState::add_tower(const TowerType &) {

}

void WorldState::update_world_state() {
    // Update bullets in the world
	for (Bullet& bullet : bullets_) {

    }

    // Update monster state
    for (Monster& monster : monsters_) {

    }

    // Update tower state
    for (Tower& tower : towers_) {
        // Check for shoots
        const vector<double>& requested_shoots = tower.get_requested_shoots();
        if (!requested_shoots.empty()) {
            bullets_.push_back({tower.get_position(), requested_shoots[0]});
        }
        // Check for rotations
        const vector<TowerRotation>& requested_rotations = tower.get_requested_rotations();
        if (!requested_rotations.empty()) {
            const double& rotational_speed = tower.get_rotational_speed();
            double& angle = tower.get_angle();
            if (requested_rotations[0] == TowerRotation::LEFT)
                angle += rotational_speed;
            else
                angle -= rotational_speed;
        }
        tower.clear_requests();
    }
}

const std::vector<Monster>& WorldState::get_monsters() const {
    return monsters_;
}

void WorldState::append_line_to_path(Position<int> src, Position<int> dst) {
    if (src.get_x() != dst.get_x()) {
        if (src.get_x() < dst.get_x()) {
            for (size_t i = src.get_x(); i <= dst.get_x(); i++) {
                if (map_[i][src.get_y()] != PATH)
                    path_.push_back(Position<int>(i, src.get_y()));
                map_[i][src.get_y()] = PATH;
            }
        } else {
            for (size_t i = src.get_x(); i >= dst.get_x(); i--) {
                if (map_[i][src.get_y()] != PATH)
                    path_.push_back(Position<int>(i, src.get_y()));
                map_[i][src.get_y()] = PATH;
            }
        }
    } else if (src.get_y() != dst.get_y()) {
        if (src.get_y() < dst.get_y()) {
            for (size_t i = src.get_y(); i <= dst.get_y(); i++) {
                if (map_[src.get_x()][i] != PATH)
                    path_.push_back(Position<int>(src.get_x(), i));
                map_[src.get_x()][i] = PATH;
            }
        } else {
            for (size_t i = src.get_y(); i >= dst.get_y(); i--) {
                if (map_[src.get_x()][i] != PATH)
                    path_.push_back(Position<int>(src.get_x(), i));
                map_[src.get_x()][i] = PATH;
            }
        }
    }
}

ostream& operator<<(ostream& os, const WorldState& obj)
{
    for (int y = 0; y < obj.height_; y++) {
        for (int x = 0; x < obj.width_; x++) {
            os << obj.map_[x][y] << " ";
        }
        os << endl;
    }
    /*os << "Path: ";
    for (const Position<int>& pos : obj.path_) {
        os << "(" << pos << "); ";
    }
    os << endl;*/
    return os;
}

