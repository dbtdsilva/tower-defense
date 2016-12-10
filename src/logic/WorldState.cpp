#include "WorldState.h"
#include <cmath>

using namespace std;

WorldState::WorldState(size_t width, size_t height) :
        width_(width), height_(height), map_(width, std::vector<PositionState>(height, EMPTY)),
        units_per_cell(500), user_interaction_(this), player_currency(10000)
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

std::vector<EntityModification> WorldState::update_world_state() {
    std::vector<EntityModification> entity_modifications;

    for (const TowerAddRequest& request : user_interaction_.get_tower_add_requests()) {
        Tower ref = Tower::create_tower(this, request.type, request.position);
        if (ref.get_cost() > player_currency)
            continue;
        player_currency -= ref.get_cost();

        entity_modifications.push_back(EntityModification(ref.get_interface(), ref.get_identifier(), EntityAction::ADD));
        towers_.push_back(std::move(ref));
        map_[request.position.get_x()][request.position.get_y()] = TOWER;
    }
    user_interaction_.clear_requests();

    // Update bullets in the world
	for (Bullet& bullet : bullets_) {
        double new_x = bullet.position.get_x() + cos(bullet.angle) * bullet.speed;
        double new_y = bullet.position.get_y() + sin(bullet.angle) * bullet.speed;

        bullet.position.set_x(new_x);
        bullet.position.set_y(new_y);
    }

    // Update monster state
    for (Monster& monster : monsters_) {
        const vector<MonsterMovement>& movements = monster.get_requested_movements();
        if (!movements.empty()) {
            const Position<double>& position = monster.get_position();
            double& angle = monster.get_angle();
            switch (movements[0]) {
                case MonsterMovement::FRONT:
                    monster.set_position(position.get_x() + cos(angle) * monster.get_movement_speed(),
                                         position.get_y() + sin(angle) * monster.get_movement_speed());
                    break;
                case MonsterMovement::BACK:
                    monster.set_position(position.get_x() + cos(angle - M_PI) * monster.get_movement_speed(),
                                         position.get_y() + sin(angle - M_PI) * monster.get_movement_speed());
                    break;
                case MonsterMovement::LEFT:
                    monster.set_position(position.get_x() + cos(angle + M_PI_2) * monster.get_movement_speed(),
                                         position.get_y() + sin(angle + M_PI_2) * monster.get_movement_speed());
                    break;
                case MonsterMovement::RIGHT:
                    monster.set_position(position.get_x() + cos(angle - M_PI_2) * monster.get_movement_speed(),
                                         position.get_y() + sin(angle - M_PI_2) * monster.get_movement_speed());
                    break;
            }
        }
        const vector<MonsterRotation>& rotations = monster.get_requested_rotations();
        if (!rotations.empty()) {
            double& angle = monster.get_angle();
            if (rotations[0] == MonsterRotation ::LEFT)
                angle += monster.get_rotational_speed();
            else
                angle -= monster.get_rotational_speed();
        }
        monster.clear_requests();
    }

    // Update tower state
    for (Tower& tower : towers_) {
        // Check for shoots
        const vector<double>& requested_shoots = tower.get_requested_shoots();
        if (!requested_shoots.empty()) {
            bullets_.push_back({ Position<double>(tower.get_position().get_x(), tower.get_position().get_y()),
                                requested_shoots[0], 0.1 });
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

    return entity_modifications;
}

const std::vector<Monster>& WorldState::get_monsters() const {
    return monsters_;
}

const double WorldState::get_wall_distance(const Position<double>& position, const double& direction,
                                            const double& sensor_precision) const {
    unsigned int index = 0;

    Position<int> next_position = {
            static_cast<int>(floor(position.get_x() + cos(direction) * index * sensor_precision)),
            static_cast<int>(floor(position.get_y() + sin(direction) * index * sensor_precision))};
    while (next_position.get_x() >= 0 && next_position.get_y() >= 0 &&
            next_position.get_x() < width_ && next_position.get_y() < height_ &&
            map_[next_position.get_x()][next_position.get_y()] == PATH) {
        index++;
        next_position = {
                static_cast<int>(floor(position.get_x() + cos(direction) * index * sensor_precision)),
                static_cast<int>(floor(position.get_y() + sin(direction) * index * sensor_precision))};
    }
    return sqrt(pow(next_position.get_x() - position.get_x(), 2) + pow(next_position.get_y() - position.get_y(), 2));
}

UserInteractionInterface* WorldState::get_user_interaction_interface() {
    return user_interaction_.get_user_interaction_interface();
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

