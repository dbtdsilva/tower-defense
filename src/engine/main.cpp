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

#include <rtdk.h> // Provides rt_print functions

#include "logic/WorldState.h"

using namespace std;

RT_TASK god_task_desc, user_task_desc;
RT_PIPE task_pipe;

#define TASK_MODE       0       // No flags
#define TASK_STKSZ      0       // Default stack size

#define TASK_PRIO_GOD       99
#define TASK_PRIO_USER      80
#define TASK_PRIO_TOWER     70
#define TASK_PRIO_MONSTER   60

void catch_signal(int sig) {}

void wait_for_ctrl_c(void) {
    signal(SIGTERM, catch_signal); //catch_signal is called if SIGTERM received
    signal(SIGINT, catch_signal);  //catch_signal is called if SIGINT received

    // Wait for CTRL+C or sigterm
    pause();
    
    // Will terminate
    rt_printf("Terminating ...\n");
}

void god_task(void *world_state_void) {
    int err;
    int task_period = 200000000;
    err = rt_task_set_periodic(NULL, TM_NOW, task_period);

    WorldState* world = static_cast<WorldState*>(world_state_void);
    string serialized_string;

    TowerInterface* tower;
    MonsterInterface* monster;
    rt_printf("%d %d %d %d\n", EINVAL, ENOMEM, EIDRM, ENODEV);
    for(;;) {
        err = rt_task_wait_period(NULL);

        vector<EntityModification> changes = world->update_world_state();
        for (EntityModification& change : changes) {
            if ((monster = dynamic_cast<MonsterInterface*>(change.entity_)) != nullptr) {
                // Create/delete monster task
            } else if ((tower = dynamic_cast<TowerInterface*>(change.entity_)) != nullptr) {
                // Create/delete tower task
            }
        }

        ostringstream stream_serialize;
        world->serialize_data(stream_serialize);
        string wat = stream_serialize.str();
        serialized_string = "MESSAGE" + wat;
        rt_printf("%d %d\n", wat[0], wat.size());
        err = rt_pipe_write(&task_pipe, serialized_string.c_str(), serialized_string.size(), P_NORMAL);
        if(err < 0) {
            rt_printf("Error sending world state message (error code = %d)\n", err);
            return;
        } else {
            rt_printf("state message sent successfully\n");
        }
    }  

    return;
}

void user_interaction_task(void *interface) {
    int err;
    int task_period = 10000;
    err = rt_task_set_periodic(NULL, TM_NOW, task_period);

    UserInteractionInterface* user_interface = static_cast<UserInteractionInterface*>(interface);
    /*for(;;) {
        err = rt_task_wait_period(NULL);
    }*/
    return;
}

void tower_task(void *interface) {
    int err;
    int task_period = 10000;
    err = rt_task_set_periodic(NULL, TM_NOW, task_period);

    TowerInterface* tower_interface = static_cast<TowerInterface*>(interface);
    /*for(;;) {
        err = rt_task_wait_period(NULL);
    }*/
    return;
}

void monster_task(void *interface) {
    int err;
    int task_period = 10000;
    err = rt_task_set_periodic(NULL, TM_NOW, task_period);

    MonsterInterface* tower_interface = static_cast<MonsterInterface*>(interface);
    /*for(;;) {
        err = rt_task_wait_period(NULL);
    }*/
    return;
}

int main(int argc, char** argv) {
    WorldState world(10, 10);
    UserInteractionInterface* user = world.get_user_interaction_interface();
    
    /* Perform auto-init of rt_print buffers if the task doesn't do so */
    rt_print_auto_init(1);

    /* Lock memory to prevent paging */
    mlockall(MCL_CURRENT | MCL_FUTURE);

    int err = rt_pipe_create(&task_pipe, NULL, 0, 0);
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
