#include <stdlib.h>
#include <iostream>

#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#include <native/task.h>
#include <native/pipe.h>
#include <native/sem.h>
#include <vector>
#include <map>

#include <rtdk.h> // Provides rt_print functions
#include <sstream>
#include <helpers/ViewerDataSerializer.h>
#include <cmath>

#include "logic/WorldState.h"

using namespace std;

#define DEBUG

#define TASK_MODE               0           // No flags
#define TASK_STACK_SIZE         0           // Default stack size

#define TASK_PRIORITY_GOD       90
#define TASK_PRIORITY_USER      80
#define TASK_PRIORITY_MONSTER   70
#define TASK_PRIORITY_TOWER     70

#define TASK_PERIOD_MS_GOD      25
#define TASK_PERIOD_MS_TOWER    50
#define TASK_PERIOD_MS_MONSTER  50

typedef struct TASK_SEM {
    TASK_SEM() : task(RT_TASK()), sem(RT_SEM()) {}
    RT_TASK task;
    RT_SEM sem;
} TASK_SEM;

RT_TASK god_task_desc;
TASK_SEM user_task_desc;

map<unsigned int, TASK_SEM> monsters_tasks;
map<unsigned int, TASK_SEM> towers_tasks;

RT_PIPE task_pipe_sender, task_pipe_receiver;

bool terminate_tasks = false;

void catch_signal(int) {
    terminate_tasks = true;

    rt_task_delete(&god_task_desc);
    for (auto iterator = monsters_tasks.begin(); !monsters_tasks.empty(); ) {
        rt_task_delete(&iterator->second.task);
        rt_sem_delete(&iterator->second.sem);
        iterator = monsters_tasks.erase(iterator);
    }
    for (auto iterator = towers_tasks.begin(); !towers_tasks.empty(); ) {
        rt_task_delete(&iterator->second.task);
        rt_sem_delete(&iterator->second.sem);
        iterator = towers_tasks.erase(iterator);
    }
    rt_task_delete(&user_task_desc.task);
    rt_sem_delete(&user_task_desc.sem);
}

void wait_for_ctrl_c(void) {
    signal(SIGTERM, catch_signal); //catch_signal is called if SIGTERM received
    signal(SIGINT, catch_signal);  //catch_signal is called if SIGINT received

    // Wait for CTRL+C or sigterm
    pause();
    
    // Will terminate
#ifdef DEBUG
    rt_printf("Terminating ...\n");
#endif
}

double normalize_angle(const double& angle) {
    double value = angle;
    while (value <= -M_PI) value += M_PI * 2.0;
    while (value > M_PI) value -= M_PI * 2.0;
    return value;
}

void tower_task(void *interface) {
    constexpr double limit_angle = (M_PI / 180.0) * 5.0;
    int task_period = TASK_PERIOD_MS_TOWER * 1000000;
    rt_task_set_periodic(NULL, TM_NOW, task_period);

#ifdef DEBUG
    rt_printf("Tower task started!\n");
#endif
    TowerInterface* tower_interface = static_cast<TowerInterface*>(interface);
    unsigned int identifier = tower_interface->get_identifier();

    Position<double> tower_map(tower_interface->get_position().get_x() + 0.5,
                               tower_interface->get_position().get_y() + 0.5);
    while (!terminate_tasks) {
        rt_task_wait_period(NULL);

        // Get radar sensor values
        std::vector<Position<double>> monsters = tower_interface->radar();
        if (monsters.size() != 0) {
            // Calculate angle between first monster and tower
            double opposite = -(monsters[0].get_y() - tower_map.get_y());
            double adjacent = monsters[0].get_x() - tower_map.get_x();

            double value;
            if (adjacent == 0) {    // division would be zero
                value = opposite > 0 ? M_PI_2 : -M_PI_2;
            } else {                // tan(theta) = theta + K * M_PI
                value = atan(opposite / adjacent);
                if (adjacent < 0)
                    value += M_PI;
            }
            double diff = normalize_angle(value - tower_interface->get_angle());

            rt_sem_p(&towers_tasks.find(identifier)->second.sem, TM_INFINITE);
            // Point to the right direction and shoot
            if (diff < -limit_angle)
                tower_interface->rotate(TowerRotation::RIGHT);
            else if (diff > limit_angle)
                tower_interface->rotate(TowerRotation::LEFT);

            if (fabs(diff) < M_PI_4)
                tower_interface->shoot();
            rt_sem_v(&towers_tasks.find(identifier)->second.sem);
        }
    }
}

void monster_task(void *interface) {
    int task_period = TASK_PERIOD_MS_MONSTER * 1000000;
    rt_task_set_periodic(NULL, TM_NOW, task_period);

#ifdef DEBUG
    rt_printf("Monster task started!\n");
#endif
    MonsterInterface* monster_interface = static_cast<MonsterInterface*>(interface);
    unsigned int identifier = monster_interface->get_identifier();

    enum Direction { D_STAY, D_LEFT, D_RIGHT };
    Direction dir = D_STAY;
    enum Move { M_STAY, M_FRONT };

    while (!terminate_tasks) {
        rt_task_wait_period(NULL);
        Direction final_dir = D_STAY;
        Move final_move = M_STAY;

        // Read sensors
        vector<MonsterEye> eyes = monster_interface->eyes();
        // Decision
        if (eyes.size() != 3) continue;
        if (dir == D_STAY && eyes.at(1).wall_distance < 0.5)
            dir = eyes.at(2).wall_distance > eyes.at(0).wall_distance ? D_RIGHT : D_LEFT;

        if (dir == D_LEFT && eyes.at(1).wall_distance < 2)
            final_dir = D_LEFT;
        else if (dir == D_RIGHT && eyes.at(1).wall_distance < 2)
            final_dir = D_RIGHT;
        else
            dir = D_STAY;

        if (dir == D_STAY) {
            if (eyes.at(0).wall_distance < 0.5) {
                final_dir = D_RIGHT;
                final_move = M_FRONT;
            } else if (eyes.at(2).wall_distance < 0.5) {
                final_dir = D_LEFT;
                final_move = M_FRONT;
            } else {
                final_move = M_FRONT;
            }
        }

        // Action
        rt_sem_p(&monsters_tasks.find(identifier)->second.sem, TM_INFINITE);
        if (final_dir == D_RIGHT)
            monster_interface->rotate(MonsterRotation::RIGHT);
        else if (final_dir == D_LEFT)
            monster_interface->rotate(MonsterRotation::LEFT);

        if (final_move == M_FRONT)
            monster_interface->move(MonsterMovement::FRONT);
        rt_sem_v(&monsters_tasks.find(identifier)->second.sem);
    }
}

void god_task(void *world_state_void) {
    int task_period = TASK_PERIOD_MS_GOD * 1000000;
    int err;

    rt_task_set_periodic(NULL, TM_NOW, task_period);

    WorldState* world = static_cast<WorldState*>(world_state_void);
    string serialized_string;

    bool exit_request = false;

    while(!terminate_tasks && !exit_request) {
        rt_task_wait_period(NULL);

        // Requests are used by every tasks, they are in a critical region
        for (auto iterator = towers_tasks.begin(); iterator != towers_tasks.end(); ++iterator)
            rt_sem_p(&iterator->second.sem, TM_INFINITE);
        for (auto iterator = monsters_tasks.begin(); iterator != monsters_tasks.end(); ++iterator)
            rt_sem_p(&iterator->second.sem, TM_INFINITE);
        // Transfer requests to the World buffer
        world->clear_world_requests();
        // Leave critical region, requests stored in the world buffer
        for (auto iterator = towers_tasks.begin(); iterator != towers_tasks.end(); ++iterator)
            rt_sem_v(&iterator->second.sem);
        for (auto iterator = monsters_tasks.begin(); iterator != monsters_tasks.end(); ++iterator)
            rt_sem_v(&iterator->second.sem);

        // Update the world state according with the requests
        vector<EntityModification> changes = world->update_world_state();
        // Process changes on entities, requests to add tasks or remove
        for (EntityModification& change : changes) {
            // Process a create/delete monster task
            if (change.type_ == EntityType::MONSTER) {
                if (change.action_ == EntityAction::ADD) {
                    monsters_tasks.insert({ change.identifier_, TASK_SEM()});
                    string task_name("Monster Task " + to_string(change.identifier_));
                    string sem_name("Monster Sem " + to_string(change.identifier_));
                    err = rt_sem_create(&monsters_tasks.find(change.identifier_)->second.sem, sem_name.c_str(), 1, S_FIFO);
#ifdef DEBUG
                    if (err) rt_printf("Error creating semaphore monster (error code = %d)\n", err);
#endif
                    err = rt_task_create(&monsters_tasks.find(change.identifier_)->second.task, task_name.c_str(),
                                             TASK_STACK_SIZE, TASK_PRIORITY_MONSTER, TASK_MODE);
                    if (err) {
#ifdef DEBUG
                        rt_printf("Error creating task monster (error code = %d)\n", err);
#endif
                    } else  {
#ifdef DEBUG
                        rt_printf("Task monster %d created successfully\n", change.identifier_);
#endif
                        rt_task_start(&monsters_tasks.find(change.identifier_)->second.task, &monster_task, change.entity_);
                    }
                } else {
#ifdef DEBUG
                    rt_printf("Deleting task monster with id %d\n", change.identifier_);
#endif
                    auto task_sem = monsters_tasks.find(change.identifier_);
                    rt_task_delete(&task_sem->second.task);
                    rt_sem_delete(&task_sem->second.sem);
                    world->delete_monster(change.identifier_);
                    monsters_tasks.erase(change.identifier_);
                }
            // Process a tower request to add or remove
            } else if (change.type_ == EntityType::TOWER) {
                if (change.action_ == EntityAction::ADD) {
                    towers_tasks.insert({change.identifier_, TASK_SEM() });
                    string task_name("Towers Task " + to_string(change.identifier_));
                    string sem_name("Towers Sem " + to_string(change.identifier_));
                    err = rt_sem_create(&towers_tasks.find(change.identifier_)->second.sem, sem_name.c_str(), 1, S_FIFO);
#ifdef DEBUG
                    if (err) rt_printf("Error creating semaphore tower (error code = %d)\n", err);
#endif
                    err = rt_task_create(&towers_tasks.find(change.identifier_)->second.task, task_name.c_str(),
                                             TASK_STACK_SIZE, TASK_PRIORITY_TOWER, TASK_MODE);
                    if(err) {
#ifdef DEBUG
                        rt_printf("Error creating task tower (error code = %d)\n", err);
#endif
                    } else  {
#ifdef DEBUG
                        rt_printf("Task tower %d created successfully\n", change.identifier_);
#endif
                        rt_task_start(&towers_tasks.find(change.identifier_)->second.task, &tower_task, change.entity_);
                    }
                } else {
#ifdef DEBUG
                    rt_printf("Deleting task tower with id %d\n", change.identifier_);
#endif
                    auto task_sem = towers_tasks.find(change.identifier_);
                    rt_task_delete(&task_sem->second.task);
                    rt_sem_delete(&task_sem->second.sem);
                    world->delete_tower(change.identifier_);
                    towers_tasks.erase(change.identifier_);
                }
            } else if (change.type_ == EntityType::USER_INTERACTION && change.action_ == EntityAction::REMOVE) {
#ifdef DEBUG
                rt_printf("Deleting user interaction\n", change.identifier_);
#endif
                rt_task_delete(&user_task_desc.task);
                rt_sem_delete(&user_task_desc.sem);
                raise(SIGINT);
            }
        }

        ostringstream stream_serialize;
        world->serialize_data(stream_serialize);
        serialized_string = "MESSAGE" + stream_serialize.str();
        rt_pipe_write(&task_pipe_sender, serialized_string.c_str(), serialized_string.size(), P_NORMAL);
    }
}

void user_interaction_task(void *interface) {
    UserInteractionInterface* user_interface = static_cast<UserInteractionInterface*>(interface);
    unsigned int buffer_size = 512;
    char buffer[buffer_size];
    string raw_received;
    while (!terminate_tasks) {
        // Read data from the pipe
        ssize_t bytes_read = rt_pipe_read(&task_pipe_receiver, buffer, buffer_size, TM_INFINITE);
        if (bytes_read <= 0)
            continue;
        raw_received = string(buffer, bytes_read);

        // Unserialize the data to the ViewerData object
        unique_ptr<ViewerData> viewer;
        istringstream file_data(raw_received);
        cereal::BinaryInputArchive archive(file_data);
        archive(viewer);

        // Process the data received
        switch (viewer->get_type()) {
            case ViewerRequest::GAME_STATUS: {
                GameStatusData* game_data = dynamic_cast<GameStatusData*>(viewer.get());
                rt_sem_p(&user_task_desc.sem, TM_INFINITE);
                user_interface->modify_game_status(game_data->status_);
                rt_sem_v(&user_task_desc.sem);
                break;
            }
            case ViewerRequest::TOWER: {
                OperationTowerData* data = dynamic_cast<OperationTowerData*>(viewer.get());
                if (data->operation_ == TowerOperation::INSERT) {
                    rt_sem_p(&user_task_desc.sem, TM_INFINITE);
                    user_interface->add_tower(data->type_, data->position_);
                    rt_sem_v(&user_task_desc.sem);
                } else {
                    rt_sem_p(&user_task_desc.sem, TM_INFINITE);
                    user_interface->remove_tower(data->position_);
                    rt_sem_v(&user_task_desc.sem);
                }
                break;
            }
        }
    }
}

int main(int argc, char** argv) {
    // WorldState(size_t width, size_t height, unsigned int god_task_period_ms, unsigned int time_between_level,
    //            unsigned int time_between_monster, unsigned int max_monsters, unsigned int max_towers);
    WorldState world(10, 10, TASK_PERIOD_MS_GOD, 5000, 750, 10, 10);
    UserInteractionInterface* user = world.get_user_interaction_interface();
    
    /* Perform auto-init of rt_print buffers if the task doesn't do so */
    rt_print_auto_init(1);

    /* Lock memory to prevent paging */
    mlockall(MCL_CURRENT | MCL_FUTURE);
    int err;

    err = rt_pipe_create(&task_pipe_sender, "Sender", 0, 1 * 1024 * 1024);         // 1 MBytes
#ifdef DEBUG
    if (err) {
        rt_printf("Error creating pipe (error code = %d)\n", err);
        return err;
    } else {
        rt_printf("Pipe created successfully\n");
    }
#endif
    err = rt_pipe_create(&task_pipe_receiver, "Receiver", 1, 1 * 1024 * 1024);     // 1 MBytes
#ifdef DEBUG
    if (err) {
        rt_printf("Error creating pipe (error code = %d)\n", err);
        return err;
    } else {
        rt_printf("Pipe created successfully\n");
    }
#endif

    /* Create RT task */
    /* Args: descriptor, name, stack size, prioritry [0..99] and mode (flags for CPU, FPU, joinable ...) */
    err = rt_task_create(&god_task_desc, "God Task", TASK_STACK_SIZE, TASK_PRIORITY_GOD, TASK_MODE);
#ifdef DEBUG
    if(err) {
        rt_printf("Error creating task a (error code = %d)\n", err);
        return err;
    } else  {
        rt_printf("God task created successfully\n");
    }
#endif
       
    err = rt_task_create(&user_task_desc.task, "User Interaction Task", TASK_STACK_SIZE, TASK_PRIORITY_USER, TASK_MODE);
#ifdef DEBUG
    if(err) {
        rt_printf("Error creating task a (error code = %d)\n", err);
        return err;
    } else  {
        rt_printf("User Interaction task created successfully\n");
    }
#endif
    err = rt_sem_create(&user_task_desc.sem, "User Task", 1, S_FIFO);
#ifdef DEBUG
    if (err) {
        rt_printf("Error creating semaphore (error code = %d)\n", err);
        return err;
    } else {
        rt_printf("Semaphore created successfully\n");
    }
#endif
        
    /* Start RT tasks */
    rt_task_start(&god_task_desc, &god_task, &world);
    rt_task_start(&user_task_desc.task, &user_interaction_task, user);

    /* wait for termination signal */   
    wait_for_ctrl_c();

    return EXIT_SUCCESS;
}
