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

#include "../engine/logic/WorldState.h"

using namespace std;

#define DEBUG

#define TASK_MODE               0           // No flags
#define TASK_STACK_SIZE         0           // Default stack size

#define TASK_PRIORITY_MONSTER       70
#define TASK_PERIOD_MS_MONSTER      50


RT_TASK task_desc;
RT_PIPE task_pipe_sender, task_pipe_receiver;

bool terminate_tasks = false;

void calculate_worst_time_monster(void* world_state_void) {
    WorldState* world = static_cast<WorldState*>(world_state_void);
    UserInteractionInterface* user = world->get_user_interaction_interface();

    MonsterInterface* monster_interface = nullptr;
    string serialized_string;
    enum Direction { D_STAY, D_LEFT, D_RIGHT };
    Direction dir = D_STAY;
    enum Move { M_STAY, M_FRONT };
    RT_SEM sem_monster;

    Direction old_dir;
    while (!terminate_tasks) {
        world->clear_world_requests();
        auto changes = world->update_world_state();

        for (EntityModification& change : changes) {
            if (change.type_ == EntityType::MONSTER && monster_interface == nullptr) {
                monster_interface = static_cast<MonsterInterface*>(change.entity_);
            }
            if (change.action_ == EntityAction::REMOVE) {
                terminate_tasks = true;
            }
        }

        if (monster_interface != nullptr){
            Direction final_dir = D_STAY;
            Move final_move = M_STAY;
            old_dir = dir;

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
            rt_sem_p(&sem_monster, TM_INFINITE);
            if (final_dir == D_RIGHT)
                monster_interface->rotate(MonsterRotation::RIGHT);
            else if (final_dir == D_LEFT)
                monster_interface->rotate(MonsterRotation::LEFT);

            if (final_move == M_FRONT)
                monster_interface->move(MonsterMovement::FRONT);
            rt_sem_v(&sem_monster);

            if (old_dir == D_STAY && dir == D_STAY && final_dir == D_LEFT && final_move == M_FRONT) {
                rt_printf("Worst case execution time and worst resource usage case!\n");
            }
        }
        ostringstream stream_serialize;
        world->serialize_data(stream_serialize);
        serialized_string = "MESSAGE" + stream_serialize.str();
        rt_pipe_write(&task_pipe_sender, serialized_string.c_str(), serialized_string.size(), P_NORMAL);
    }
    rt_printf("Monster has finished!\n");
}

void catch_signal(int) {
    terminate_tasks = true;

    rt_task_delete(&task_desc);
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

int main(int argc, char** argv) {
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
    err = rt_task_create(&task_desc, "God Task", TASK_STACK_SIZE, TASK_PRIORITY_MONSTER, TASK_MODE);
#ifdef DEBUG
    if(err) {
        rt_printf("Error creating task a (error code = %d)\n", err);
        return err;
    } else  {
        rt_printf("God task created successfully\n");
    }
#endif

    /* Start RT tasks */
    WorldState world(10, 10, 25, 5000, 750, 10, 10);
    rt_task_start(&task_desc, &calculate_worst_time_monster, &world);
    //rt_task_start(&user_task_desc.task, &user_interaction_task, user);
    /* wait for termination signal */
    wait_for_ctrl_c();

    return EXIT_SUCCESS;
}
