#include <stdlib.h>
#include <iostream>

#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#include <native/task.h>
#include <native/pipe.h>
#include <native/sem.h>
#include <vector>

#include <rtdk.h> // Provides rt_print functions
#include <sstream>

#include "logic/WorldState.h"
#include "logic/monster/MonsterEye.h"

using namespace std;

RT_TASK god_task_desc, user_task_desc;
vector<RT_TASK> monsters_tasks;
vector<RT_TASK> towers_tasks;

RT_PIPE task_pipe_sender, task_pipe_receiver;
RT_SEM sem_critical_region;

#define TASK_MODE       0       // No flags
#define TASK_STKSZ      0       // Default stack size

#define TASK_PRIORITY_GOD       99
#define TASK_PRIORITY_USER      80
#define TASK_PRIORITY_TOWER     70
#define TASK_PRIORITY_MONSTER   60

#define TASK_PERIOD_MS_GOD      20
#define TASK_PERIOD_MS_TOWER    20
#define TASK_PERIOD_MS_MONSTER  20

bool terminate_tasks = false;

void catch_signal(int) {
    terminate_tasks = true;
}

void wait_for_ctrl_c(void) {
    signal(SIGTERM, catch_signal); //catch_signal is called if SIGTERM received
    signal(SIGINT, catch_signal);  //catch_signal is called if SIGINT received

    // Wait for CTRL+C or sigterm
    pause();
    
    // Will terminate
    rt_printf("Terminating ...\n");
}

void tower_task(void *interface) {
    int task_period = TASK_PERIOD_MS_TOWER * 1000000;
    rt_task_set_periodic(NULL, TM_NOW, task_period);

    TowerInterface* tower_interface = static_cast<TowerInterface*>(interface);
    while (!terminate_tasks) {
        rt_task_wait_period(NULL);

        rt_sem_p(&sem_critical_region, TM_INFINITE);
        tower_interface->shoot();
        rt_sem_v(&sem_critical_region);
    }
}

void monster_task(void *interface) {
    int task_period = TASK_PERIOD_MS_MONSTER * 1000000;
    rt_task_set_periodic(NULL, TM_NOW, task_period);

    rt_printf("Monster task started!\n");
    MonsterInterface* monster_interface = static_cast<MonsterInterface*>(interface);

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
        if (dir == D_STAY && monster_interface->eyes()[1].wall_distance < 0.5) {
            dir = monster_interface->eyes()[2].wall_distance > monster_interface->eyes()[0].wall_distance ?
                  D_RIGHT : D_LEFT;
        }

        if (dir == D_LEFT && monster_interface->eyes()[1].wall_distance < 2) {
            final_dir = D_LEFT;
        } else if (dir == D_RIGHT && monster_interface->eyes()[1].wall_distance < 2) {
            final_dir = D_RIGHT;
        } else {
            dir = D_STAY;
        }

        if (dir == D_STAY) {
            if (monster_interface->eyes()[0].wall_distance < 0.5) {
                final_dir = D_RIGHT;
                final_move = M_FRONT;
            } else if (monster_interface->eyes()[2].wall_distance < 0.5) {
                final_dir = D_LEFT;
                final_move = M_FRONT;
            } else {
                final_move = M_FRONT;
            }
        }

        // Action
        rt_sem_p(&sem_critical_region, TM_INFINITE);
        if (final_dir == D_RIGHT)
            monster_interface->rotate(MonsterRotation::RIGHT);
        else if (final_dir == D_LEFT)
            monster_interface->rotate(MonsterRotation::LEFT);

        if (final_move == M_FRONT)
            monster_interface->move(MonsterMovement::FRONT);
        rt_sem_v(&sem_critical_region);
    }
    return;
}

void god_task(void *world_state_void) {
    int task_period = TASK_PERIOD_MS_GOD * 1000000;

    rt_task_set_periodic(NULL, TM_NOW, task_period);

    WorldState* world = static_cast<WorldState*>(world_state_void);
    string serialized_string;

    MonsterInterface* monster;
    TowerInterface* tower;

    rt_printf("%d %d %d %d\n", EINVAL, ENOMEM, EIDRM, ENODEV);
    while(!terminate_tasks) {
        rt_task_wait_period(NULL);

        vector<EntityModification> changes = world->update_world_state();

        rt_sem_p(&sem_critical_region, TM_INFINITE);
        world->clear_world_requests();
        rt_sem_v(&sem_critical_region);

        for (EntityModification& change : changes) {
            if ((monster = dynamic_cast<MonsterInterface*>(change.entity_)) != nullptr) {
                // Create/delete monster task
                monsters_tasks.push_back(RT_TASK());
                string task_name("Monster Task " + monsters_tasks.size());
                int err = rt_task_create(&monsters_tasks.back(), task_name.c_str(), TASK_STKSZ, TASK_PRIORITY_MONSTER, TASK_MODE);
                if(err) {
                    rt_printf("Error creating task monster (error code = %d)\n", err);
                } else  {
                    rt_printf("Task created successfully\n");
                    rt_task_start(&monsters_tasks.back(), &monster_task, monster);
                }
            } else if ((tower = dynamic_cast<TowerInterface*>(change.entity_)) != nullptr) {
                towers_tasks.push_back(RT_TASK());
                string task_name("Towers Task " + towers_tasks.size());
                int err = rt_task_create(&towers_tasks.back(), task_name.c_str(), TASK_STKSZ, TASK_PRIORITY_TOWER, TASK_MODE);
                if(err) {
                    rt_printf("Error creating task tower (error code = %d)\n", err);
                } else  {
                    rt_printf("Task created successfully\n");
                    rt_task_start(&monsters_tasks.back(), &tower_task, tower);
                }
            }
        }

        ostringstream stream_serialize;
        world->serialize_data(stream_serialize);
        serialized_string = "MESSAGE" + stream_serialize.str();
        ssize_t err = rt_pipe_write(&task_pipe_sender, serialized_string.c_str(), serialized_string.size(), P_NORMAL);
        if(err < 0) {
            rt_printf("Error sending world state message (error code = %d)\n", err);
            return;
        }

    }
}

void user_interaction_task(void *interface) {
    UserInteractionInterface* user_interface = static_cast<UserInteractionInterface*>(interface);
    while (!terminate_tasks) {
        rt_pipe_read(&task_pipe_receiver, nullptr, 0, TM_INFINITE);
    }
}

int main(int argc, char** argv) {
    WorldState world(10, 10, TASK_PERIOD_MS_GOD);
    UserInteractionInterface* user = world.get_user_interaction_interface();
    
    /* Perform auto-init of rt_print buffers if the task doesn't do so */
    rt_print_auto_init(1);

    /* Lock memory to prevent paging */
    mlockall(MCL_CURRENT | MCL_FUTURE);

    int err = rt_pipe_create(&task_pipe_sender, NULL, 0, 0);
    if (err) {
        rt_printf("Error creating pipe (error code = %d)\n", err);
        return err;
    } else {
        rt_printf("Pipe created successfully\n");
    }

    err = rt_pipe_create(&task_pipe_receiver, NULL, 1, 0);
    if (err) {
        rt_printf("Error creating pipe (error code = %d)\n", err);
        return err;
    } else {
        rt_printf("Pipe created successfully\n");
    }

    err = rt_sem_create(&sem_critical_region, "CriticalRegion", 1, S_FIFO);
    if (err) {
        rt_printf("Error creating semaphore (error code = %d)\n", err);
        return err;
    } else {
        rt_printf("Semaphore created successfully\n");
    }
    /* Create RT task */
    /* Args: descriptor, name, stack size, prioritry [0..99] and mode (flags for CPU, FPU, joinable ...) */
    err = rt_task_create(&god_task_desc, "God Task", TASK_STKSZ, TASK_PRIORITY_GOD, TASK_MODE);
    if(err) {
        rt_printf("Error creating task a (error code = %d)\n", err);
        return err;
    } else  {
        rt_printf("God task created successfully\n");
    }
       
    err = rt_task_create(&user_task_desc, "User Interaction Task", TASK_STKSZ, TASK_PRIORITY_USER, TASK_MODE);
    if(err) {
        rt_printf("Error creating task a (error code = %d)\n", err);
        return err;
    } else  {
        rt_printf("User Interaction task created successfully\n");
    }
        
    /* Start RT tasks */
    rt_task_start(&god_task_desc, &god_task, &world);
    rt_task_start(&user_task_desc, &user_interaction_task, user); 

    /* wait for termination signal */   
    wait_for_ctrl_c();

    return EXIT_SUCCESS;
}
