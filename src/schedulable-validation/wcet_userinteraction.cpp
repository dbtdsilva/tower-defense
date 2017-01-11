#include <stdlib.h>
#include <iostream>

#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#include <native/task.h>
#include <native/pipe.h>
#include <native/sem.h>
#include <vector>
#include <sys/time.h>
#include <climits>

#include <rtdk.h> // Provides rt_print functions
#include <sstream>
#include <helpers/ViewerDataSerializer.h>
#include <cmath>

#include "../engine/logic/WorldState.h"

using namespace std;

#define TASK_MODE               0           // No flags
#define TASK_STACK_SIZE         0           // Default stack size

#define TASK_PRIORITY_USER       80

RT_TASK task_desc;
RT_PIPE task_pipe_sender, task_pipe_receiver;

bool terminate_tasks = false;

enum TimerState { BOOTING, NORMAL };

double normalize_angle(const double& angle) {
    double value = angle;
    while (value <= -M_PI) value += M_PI * 2.0;
    while (value > M_PI) value -= M_PI * 2.0;
    return value;
}

void calculate_worst_time_monster(void* world_state_void) {
    WorldState* world = static_cast<WorldState*>(world_state_void);
    UserInteractionInterface* user_interface = world->get_user_interaction_interface();

    unsigned int buffer_size = 512;
    char buffer[buffer_size];
    string raw_received;

    string serialized_string;
    RT_SEM sem_user;

    Position<double> tower_map;
    int task_period = 25 * 1000000;
    rt_task_set_periodic(NULL, TM_NOW, task_period);
    static TimerState state = BOOTING; // State initialization
    static int activ_counter = 0; // Activation counter
    static long tmaxus, tminus = LONG_MAX;

    struct timeval tcur, tend, tdif;

    while (!terminate_tasks) {
        rt_task_wait_period(NULL);

        ostringstream stream_serialize;
        world->serialize_data(stream_serialize);
        serialized_string = "MESSAGE" + stream_serialize.str();
        rt_pipe_write(&task_pipe_sender, serialized_string.c_str(), serialized_string.size(), P_NORMAL);

        world->clear_world_requests();
        auto changes = world->update_world_state();

        // User interaction task flow starts HERE
        // Read data from the pipe
        // The buffer will not vary within the same type of request!!
        ssize_t bytes_read = rt_pipe_read(&task_pipe_receiver, buffer, buffer_size, TM_NONBLOCK);
#ifdef WCET
        gettimeofday(&tcur, NULL);
#endif
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
                rt_sem_p(&sem_user, TM_INFINITE);
#ifdef BLOCKING
                gettimeofday(&tcur, NULL);
#endif
                user_interface->modify_game_status(game_data->status_);
#ifdef BLOCKING
                gettimeofday(&tend, NULL);
#endif
                rt_sem_v(&sem_user);
                break;
            }
            case ViewerRequest::TOWER: {
                OperationTowerData* data = dynamic_cast<OperationTowerData*>(viewer.get());
                if (data->operation_ == TowerOperation::INSERT) {
                    rt_sem_p(&sem_user, TM_INFINITE);
#ifdef BLOCKING
                    gettimeofday(&tcur, NULL);
#endif
                    user_interface->add_tower(data->type_, data->position_);
#ifdef BLOCKING
                    gettimeofday(&tend, NULL);
#endif
                    rt_sem_v(&sem_user);
                } else {
                    rt_sem_p(&sem_user, TM_INFINITE);
#ifdef BLOCKING
                    gettimeofday(&tcur, NULL);
#endif
                    user_interface->remove_tower(data->position_);
#ifdef BLOCKING
                    gettimeofday(&tend, NULL);
#endif
                    rt_sem_v(&sem_user);
                }
                break;
            }
        }
#ifdef WCET
        gettimeofday(&tend, NULL);
#endif
        timersub(&tend, &tcur, &tdif);
        if (state == BOOTING) {
            activ_counter++;
            if (activ_counter > 5)
                state = NORMAL;
        } if(state == NORMAL) {
            if (tdif.tv_usec > tmaxus)
                tmaxus = tdif.tv_usec;
            if (tdif.tv_usec < tminus)
                tminus = tdif.tv_usec;
            printf("Last instance period = %6ld Maximum = %6ld Minumum = %6ld (us), Seconds: \n", tdif.tv_usec, tmaxus, tminus);
        }
        // FLOW ENDS HERE
        rt_printf("Received data in the user interaction!\n");
    }
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
    err = rt_task_create(&task_desc, "God Task", TASK_STACK_SIZE, TASK_PRIORITY_USER, TASK_MODE);
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
