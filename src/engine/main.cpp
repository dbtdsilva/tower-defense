#include <stdlib.h>
#include <iostream>
#include <fstream>

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

#define TASK_PRIO_GOD   99
#define TASK_PRIO_USER  80

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
    WorldState* world = static_cast<WorldState*>(world_state_void);

    cout << *world << endl;
}

void user_interaction_task(void *interface) {
    UserInteractionInterface* user_interface = static_cast<UserInteractionInterface*>(interface);
}

int main(int argc, char** argv) {
    WorldState world(10, 10);
    UserInteractionInterface* user = world.get_user_interaction_interface();
    /*user->add_tower(TowerType::SIMPLE, Position<int>(2,0));
    cout << state << endl;
    for (int i = 0; i < 40; i++) {
        cout << i << endl;
        vector<EntityModification> changes = state.update_world_state();
        for (EntityModification& change : changes) {
            TowerInterface* tower;
            MonsterInterface* monster;
            cout << "Action: " << (int)change.action_ << endl;
            if ((monster = dynamic_cast<MonsterInterface*>(change.entity_)) != nullptr) {
                cout << "Monster" << endl;
            } else if ((tower = dynamic_cast<TowerInterface*>(change.entity_)) != nullptr) {
                cout << "Tower" << endl;
                tower->radar();
                tower->shoot();
            }
        }
    }

    ofstream world_serialize;
    world_serialize.open("world_serialized.bin", ios::binary);
    state.serialize_data(world_serialize);
    world_serialize.close();*/

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
