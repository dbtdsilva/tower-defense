#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include <unistd.h>
#include <signal.h>
#include <sys/mman.h> // For mlockall
#include <sys/types.h>

#include <native/task.h>
#include <native/timer.h>
#include <native/pipe.h>
#include <vector>

#include <rtdk.h> // Provides rt_print functions
#include <sstream>

#include "logic/WorldState.h"

using namespace std;

RT_TASK god_task_desc, user_task_desc;
vector<RT_TASK> monsters_tasks;
vector<RT_TASK> towers_tasks;

RT_PIPE task_pipe_sender, task_pipe_receiver;

int god_task_period_ms = 20;

#define TASK_MODE       0       // No flags
#define TASK_STKSZ      0       // Default stack size

#define TASK_PRIO_GOD       99
#define TASK_PRIO_USER      80
#define TASK_PRIO_TOWER     70
#define TASK_PRIO_MONSTER   60

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
    int task_period = 20000000;
    rt_task_set_periodic(NULL, TM_NOW, task_period);

    TowerInterface* tower_interface = static_cast<TowerInterface*>(interface);
    while (!terminate_tasks) {
        rt_task_wait_period(NULL);
        tower_interface->shoot();
    }
}

void monster_task(void *interface) {
    int task_period = 20000000;
    rt_task_set_periodic(NULL, TM_NOW, task_period);

    rt_printf("Monster task started!\n");
    MonsterInterface* monster_interface = static_cast<MonsterInterface*>(interface);
    while (!terminate_tasks) {
        rt_task_wait_period(NULL);
        monster_interface->move(MonsterMovement::FRONT);
    }
    return;
}

void god_task(void *world_state_void) {
    int task_period = god_task_period_ms * 1000000;

    rt_task_set_periodic(NULL, TM_NOW, task_period);

    WorldState* world = static_cast<WorldState*>(world_state_void);
    string serialized_string;

    MonsterInterface* monster;
    TowerInterface* tower;

    rt_printf("%d %d %d %d\n", EINVAL, ENOMEM, EIDRM, ENODEV);
    while(!terminate_tasks) {
        rt_task_wait_period(NULL);

        vector<EntityModification> changes = world->update_world_state();
        for (EntityModification& change : changes) {
            if ((monster = dynamic_cast<MonsterInterface*>(change.entity_)) != nullptr) {
                // Create/delete monster task
                monsters_tasks.push_back(RT_TASK());
                string task_name("Monster Task " + monsters_tasks.size());
                int err = rt_task_create(&monsters_tasks.back(), task_name.c_str(), TASK_STKSZ, TASK_PRIO_MONSTER, TASK_MODE);
                if(err) {
                    rt_printf("Error creating task monster (error code = %d)\n", err);
                } else  {
                    rt_printf("Task created successfully\n");
                    rt_task_start(&monsters_tasks.back(), &monster_task, monster);
                }
            } else if ((tower = dynamic_cast<TowerInterface*>(change.entity_)) != nullptr) {
                towers_tasks.push_back(RT_TASK());
                string task_name("Towers Task " + towers_tasks.size());
                int err = rt_task_create(&towers_tasks.back(), task_name.c_str(), TASK_STKSZ, TASK_PRIO_TOWER, TASK_MODE);
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
        } else {
            rt_printf("state message sent successfully\n");
        }
    }
}

void user_interaction_task(void *interface) {
    int task_period = 20000000;
    rt_task_set_periodic(NULL, TM_NOW, task_period);

    UserInteractionInterface* user_interface = static_cast<UserInteractionInterface*>(interface);
    while (!terminate_tasks) {
        rt_task_wait_period(NULL);
        //rt_pipe_read(&task_pipe_receiver, nullptr, 0, TM_INFINITE);
    }
}

int main(int argc, char** argv) {
    WorldState world(10, 10, god_task_period_ms);
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

    /* Create RT task */
    /* Args: descriptor, name, stack size, prioritry [0..99] and mode (flags for CPU, FPU, joinable ...) */
    err = rt_task_create(&god_task_desc, "God Task", TASK_STKSZ, TASK_PRIO_GOD, TASK_MODE);
    if(err) {
        rt_printf("Error creating task a (error code = %d)\n", err);
        return err;
    } else  {
        rt_printf("God task created successfully\n");
    }
       
    err = rt_task_create(&user_task_desc, "User Interaction Task", TASK_STKSZ, TASK_PRIO_USER, TASK_MODE);
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
