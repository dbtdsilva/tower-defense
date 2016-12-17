#include "WorldState.h"
#include "interaction/UserInteraction.h"

#include <cmath>
#include <native/timer.h>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <helpers/WorldDataSerializer.h>
#include <native/types.h>
#include <unistd.h>

using namespace std;

WorldState::WorldState(size_t width, size_t height, int god_task_period_ms) :
        width_(width), height_(height), map_(width, std::vector<PositionState>(height, PositionState::EMPTY)),
        user_interaction_(this), player_currency_(10000),
        game_level_(0), monsters_per_level_(5), monsters_left_to_spawn_(0), idle_cycles_(0),
        idle_cycles_before_spawn_(0), start_position(0, 0), end_position(0, 0), cycle_ms_(god_task_period_ms),
        score_(0), lives_(10)
{
    start_position.set_x(0);
    start_position.set_y(1);
    append_line_to_path(Position<int>(0, 1), Position<int>(width_ - 2, 1));
    append_line_to_path(Position<int>(width_ - 2, 1), Position<int>(width_ - 2, height_ - 2));
    append_line_to_path(Position<int>(width_ - 2, height_ - 2), Position<int>(3, height_ - 2));
    append_line_to_path(Position<int>(3, height_ - 2), Position<int>(3, height_ - 4));
    append_line_to_path(Position<int>(3, height_ - 4), Position<int>(width_ - 4, height_ - 4));
    append_line_to_path(Position<int>(width_ - 4, height_ - 4), Position<int>(width_ - 4, 3));
    append_line_to_path(Position<int>(width_ - 4, 3), Position<int>(1, 3));
    append_line_to_path(Position<int>(1, 3), Position<int>(1, height_ - 1));
    end_position.set_x(1);
    end_position.set_y(height - 1);
}

void WorldState::clear_world_requests() {
    user_interaction_.clear_requests();
    for (auto& monster : monsters_) {
        monster->clear_requests();
    }
    for (auto& tower : towers_) {
        tower->clear_requests();
    }
}

void WorldState::delete_tower(unsigned int identifier) {
    for (auto towers_iter = towers_.begin(); towers_iter != towers_.end(); ++towers_iter) {
        Tower * tower = towers_iter->get();
        if (tower->get_identifier() == identifier) {
            map_[tower->get_position().get_x()][tower->get_position().get_y()] = PositionState::EMPTY;
            player_currency_ += tower->get_cost() / 2.0;
            towers_.erase(towers_iter);
            break;
        }
    }
}

void WorldState::delete_monster(unsigned int identifier) {
    for (auto monster_iter = monsters_.begin(); monster_iter != monsters_.end(); ++monster_iter) {
        if (monster_iter->get()->get_identifier() == identifier) {
            monsters_.erase(monster_iter);
            break;
        }
    }
}

std::vector<EntityModification> WorldState::update_world_state() {
    std::vector<EntityModification> entity_modifications;
    if (lives_ == 0)
        return entity_modifications;

    if (monsters_left_to_spawn_ == 0) {
        if (monsters_.empty()) {
            idle_cycles_++;
            if (idle_cycles_ > (2.0 / cycle_ms_)) {
                game_level_++;
                monsters_left_to_spawn_ = monsters_per_level_;
                idle_cycles_ = 0;
            }
        }
    } else if (idle_cycles_before_spawn_ > (200.0 / cycle_ms_)){
        unique_ptr<Monster> ref = make_unique<Monster>(Monster::add_monster(
                this, MonsterType::BASIC, Position<double>(start_position.get_x() + 0.5, start_position.get_y() + 0.5)));
        monsters_.push_back(std::move(ref));
        entity_modifications.push_back(EntityModification(monsters_.back()->get_interface(),
                                                          monsters_.back()->get_identifier(),
                                                          EntityAction::ADD,
                                                          EntityType::MONSTER));
        monsters_left_to_spawn_--;
        idle_cycles_before_spawn_ = 0;
    } else {
        idle_cycles_before_spawn_++;
    }

    for (const TowerRequest& request : user_interaction_.get_tower_add_requests()) {
        if (request.position.get_x() < 0 || request.position.get_x() >= width_ ||
            request.position.get_y() < 0 || request.position.get_y() >= height_)
            continue;

        if (request.operation == TowerOperation::INSERT) {
            if (map_[request.position.get_x()][request.position.get_y()] != PositionState::EMPTY)
                continue;
            unique_ptr<Tower> ref =  make_unique<Tower>(Tower::create_tower(this, request.type, request.position));
            if (ref->get_cost() > player_currency_)
                continue;
            player_currency_ -= ref->get_cost();

            towers_.push_back(std::move(ref));
            entity_modifications.push_back(EntityModification(towers_.back()->get_interface(),
                                                              towers_.back()->get_identifier(),
                                                              EntityAction::ADD,
                                                              EntityType::TOWER));
            map_[request.position.get_x()][request.position.get_y()] = PositionState::TOWER;
        } else {
            if (map_[request.position.get_x()][request.position.get_y()] == PositionState::EMPTY)
                continue;
            for (auto tower_iter = towers_.begin(); tower_iter != towers_.end(); ++tower_iter) {
                Tower * tower_requested = tower_iter->get();
                if (tower_requested->get_position() == request.position) {
                    entity_modifications.push_back(EntityModification(tower_requested->get_interface(),
                                                                      tower_requested->get_identifier(),
                                                                      EntityAction::REMOVE,
                                                                      EntityType::TOWER));
                    break;
                }
            }
        }
    }

    // Update bullets in the world
    for(auto bullet_iter = bullets_.begin(); bullet_iter != bullets_.end(); ) {
        Bullet* bullet = bullet_iter->get();
        if (bullet == nullptr) break;
        double new_x = bullet->get_position().get_x() + cos(bullet->get_angle()) * bullet->get_speed();
        double new_y = bullet->get_position().get_y() - sin(bullet->get_angle()) * bullet->get_speed();

        bullet->get_position().set_x(new_x);
        bullet->get_position().set_y(new_y);

        if (new_x < 0 || new_x >= width_ || new_y < 0 || new_y >= height_ ||
                bullet->get_distance_travelled() >= bullet->get_range()) {
            bullet_iter = bullets_.erase(bullet_iter);
            continue;
        }

        bool bullet_struck = false;
        for (auto monster_iter = monsters_.begin(); monster_iter != monsters_.end(); ++monster_iter) {
            Monster* monster = monster_iter->get();
            if (sqrt(pow(new_x - monster->get_position().get_x(), 2) +
                     pow(new_y - monster->get_position().get_y(), 2)) < 0.1) {
                bullet_struck = true;
                int health_left = monster->bullet_struck(bullet->get_damage());
                bullet_iter = bullets_.erase(bullet_iter);
                if (health_left <= 0) {
                    switch (monster->get_type()) {
                        case MonsterType::BASIC:
                            score_ += 1;
                            player_currency_ += 10;
                            break;
                        case MonsterType::INSANE:
                            score_ += 2;
                            player_currency_ += 25;
                            break;
                    }
                    entity_modifications.push_back(EntityModification(monster->get_interface(),
                                                                      monster->get_identifier(),
                                                                      EntityAction::REMOVE,
                                                                      EntityType::MONSTER));
                    break;
                }
            }
        }

        if (!bullet_struck) {
            bullet_iter++;
        }
    }

    // Update monster state
    for (auto monster_iter = monsters_.begin(); monster_iter != monsters_.end(); ) {
        Monster* monster = monster_iter->get();
        monster->update_eyes();

        const vector<MonsterMovement>& movements = monster->get_requested_movements();
        if (!movements.empty()) {
            const Position<double>& position = monster->get_position();
            double& angle = monster->get_angle();
            Position<double> new_position(0, 0);
            switch (movements[0]) {
                case MonsterMovement::FRONT:
                    new_position.set_x(position.get_x() + cos(angle) * monster->get_movement_speed());
                    new_position.set_y(position.get_y() - sin(angle) * monster->get_movement_speed());
                    break;
                case MonsterMovement::BACK:
                    new_position.set_x(position.get_x() + cos(angle - M_PI) * monster->get_movement_speed());
                    new_position.set_y(position.get_y() - sin(angle - M_PI) * monster->get_movement_speed());
                    break;
                case MonsterMovement::LEFT:
                    new_position.set_x(position.get_x() + cos(angle + M_PI_2) * monster->get_movement_speed());
                    new_position.set_y(position.get_y() - sin(angle + M_PI_2) * monster->get_movement_speed());
                    break;
                case MonsterMovement::RIGHT:
                    new_position.set_x(position.get_x() + cos(angle - M_PI_2) * monster->get_movement_speed());
                    new_position.set_y(position.get_y() - sin(angle - M_PI_2) * monster->get_movement_speed());
                    break;
            }
            Position<int> new_position_map(static_cast<int>(floor(new_position.get_x())),
                                           static_cast<int>(floor(new_position.get_y())));
            if (new_position_map.get_x() >= 0 && new_position_map.get_x() < width_ &&
                    new_position_map.get_y() >= 0 && new_position_map.get_y() < height_ &&
                    map_[new_position_map.get_x()][new_position_map.get_y()] == PositionState::PATH) {
                if (sqrt(pow(new_position.get_x() - (end_position.get_x() + 0.5), 2) +
                         pow(new_position.get_y() - (end_position.get_y() + 0.5), 2)) < 0.2) {
                    lives_ = lives_ > 0 ? lives_ - 1 : lives_;
                    entity_modifications.push_back(EntityModification(monster->get_interface(),
                                                                      monster->get_identifier(),
                                                                      EntityAction::REMOVE,
                                                                      EntityType::MONSTER));
                    continue;
                } else {
                    monster->set_position(new_position.get_x(), new_position.get_y());
                }
            }
        }
        const vector<MonsterRotation>& rotations = monster->get_requested_rotations();
        if (!rotations.empty()) {
            double& angle = monster->get_angle();
            if (rotations[0] == MonsterRotation::LEFT)
                angle += monster->get_rotational_speed();
            else
                angle -= monster->get_rotational_speed();
        }
        monster_iter++;
    }

    // Update tower state
    for (auto& tower : towers_) {
        // Check for shoots
        const vector<double>& requested_shoots = tower->get_requested_shoots();
        if (!requested_shoots.empty()) {
            bullets_.push_back(make_unique<Bullet>(Position<double>(tower->get_position().get_x() + 0.5,
                                                                    tower->get_position().get_y() + 0.5),
                                                   requested_shoots[0], 0.1, tower->get_damage(), tower->get_range()));
        }
        // Check for rotations
        const vector<TowerRotation>& requested_rotations = tower->get_requested_rotations();
        if (!requested_rotations.empty()) {
            const double& rotational_speed = tower->get_rotational_speed();
            double& angle = tower->get_angle();
            if (requested_rotations[0] == TowerRotation::LEFT)
                angle += rotational_speed;
            else
                angle -= rotational_speed;
        }
    }

    return entity_modifications;
}

void WorldState::serialize_data(ostream& stream) const {
    WorldData data_to_serialize;
    for (const auto& tower : towers_)
        data_to_serialize.towers_.push_back(TowerData(tower->get_position(), tower->get_type(), tower->get_angle()));
    for (const auto& bullet : bullets_)
        data_to_serialize.bullets_.push_back(BulletData(bullet->get_position(), bullet->get_damage()));
    for (const auto& monster : monsters_)
        data_to_serialize.monsters_.push_back(MonsterData(monster->get_position(), monster->get_type(),
                                                          monster->get_health(), monster->get_angle()));
    data_to_serialize.map_ = map_;
    data_to_serialize.score_ = score_;
    data_to_serialize.level_ = game_level_;
    data_to_serialize.monsters_left_level_ = monsters_left_to_spawn_;
    data_to_serialize.player_currency_ = player_currency_;
    data_to_serialize.lives_ = lives_;

    data_to_serialize.start_ = start_position;
    data_to_serialize.end_ = end_position;

    cereal::BinaryOutputArchive archive(stream);
    archive(data_to_serialize);
}

const std::vector<Position<double>> WorldState::get_monsters_position(
        Position<double> tower_pos, double tower_range) const {
    vector<Position<double>> monster_list;
    for (const auto& monster : monsters_) {
        if (sqrt(pow(monster.get()->get_position().get_x() - tower_pos.get_x(), 2) +
                 pow(monster.get()->get_position().get_y() - tower_pos.get_y(), 2)) < tower_range) {
            monster_list.push_back(monster.get()->get_position());
        }
    }
    return monster_list;
}

const double WorldState::get_wall_distance(const Position<double>& position, const double& direction,
                                            const double& sensor_precision) const {
    unsigned int index = 0;
    Position<int> next_position = {
            static_cast<int>(floor(position.get_x() + cos(direction) * index * sensor_precision)),
            static_cast<int>(floor(position.get_y() - sin(direction) * index * sensor_precision))};
    Position<double> valid_position = { position.get_x() + cos(direction) * index * sensor_precision,
                                        position.get_y() + sin(direction) * index * sensor_precision};
    while (next_position.get_x() >= 0 && next_position.get_y() >= 0 &&
            next_position.get_x() < width_ && next_position.get_y() < height_ &&
            map_[next_position.get_x()][next_position.get_y()] == PositionState::PATH) {
        valid_position = { position.get_x() + cos(direction) * index * sensor_precision,
                           position.get_y() - sin(direction) * index * sensor_precision};
        index++;
        next_position = {
                static_cast<int>(floor(position.get_x() + cos(direction) * index * sensor_precision)),
                static_cast<int>(floor(position.get_y() - sin(direction) * index * sensor_precision))};
    }
    return sqrt(pow(valid_position.get_x() - position.get_x(), 2) + pow(valid_position.get_y() - position.get_y(), 2));
}

UserInteractionInterface* WorldState::get_user_interaction_interface() {
    return user_interaction_.get_user_interaction_interface();
}

void WorldState::append_line_to_path(Position<int> src, Position<int> dst) {
    if (src.get_x() != dst.get_x()) {
        if (src.get_x() < dst.get_x()) {
            for (size_t i = src.get_x(); i <= dst.get_x(); i++) {
                if (map_[i][src.get_y()] != PositionState::PATH)
                    path_.push_back(Position<int>(i, src.get_y()));
                map_[i][src.get_y()] = PositionState::PATH;
            }
        } else {
            for (size_t i = src.get_x(); i >= dst.get_x(); i--) {
                if (map_[i][src.get_y()] != PositionState::PATH)
                    path_.push_back(Position<int>(i, src.get_y()));
                map_[i][src.get_y()] = PositionState::PATH;
            }
        }
    } else if (src.get_y() != dst.get_y()) {
        if (src.get_y() < dst.get_y()) {
            for (size_t i = src.get_y(); i <= dst.get_y(); i++) {
                if (map_[src.get_x()][i] != PositionState::PATH)
                    path_.push_back(Position<int>(src.get_x(), i));
                map_[src.get_x()][i] = PositionState::PATH;
            }
        } else {
            for (size_t i = src.get_y(); i >= dst.get_y(); i--) {
                if (map_[src.get_x()][i] != PositionState::PATH)
                    path_.push_back(Position<int>(src.get_x(), i));
                map_[src.get_x()][i] = PositionState::PATH;
            }
        }
    }
}

void WorldState::simulate_load(long load_ms) {
    struct timespec req = {0};
    time_t sec = (int) (load_ms / 1000);
    load_ms = load_ms - (sec * 1000);
    req.tv_sec = sec;
    req.tv_nsec = load_ms * 1000000L;

    nanosleep(&req, &req);
    /*RTIME ti, tf;
    RTIME load_ns = 10;//1000000 * (RTIME) load_ms;

    ti = rt_timer_read();           // Get initial time
    tf = ti + load_ns;              // Compute end time
    while(rt_timer_read() < tf);    // Busy wait*/
}

ostream& operator<<(ostream& os, const WorldState& obj)
{
    for (int y = 0; y < obj.height_; y++) {
        for (int x = 0; x < obj.width_; x++) {
            os << (int)obj.map_[x][y] << " ";
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

