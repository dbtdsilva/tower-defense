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
#include <sys/time.h>

#include <rtdk.h> // Provides rt_print functions
#include <sstream>
#include <helpers/ViewerDataSerializer.h>
#include <cmath>

#include "../engine/logic/WorldState.h"

using namespace std;

#define DEBUG

#define TASK_MODE               0           // No flags
#define TASK_STACK_SIZE         0           // Default stack size

#define TASK_PRIORITY_TOWER       70
#define TASK_PERIOD_MS_TOWER      50


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
    UserInteractionInterface* user = world->get_user_interaction_interface();

    string serialized_string;
    enum Direction { D_STAY, D_LEFT, D_RIGHT };
    Direction dir = D_STAY;
    enum Move { M_STAY, M_FRONT };
    RT_SEM sem_tower;

    vector<MonsterInterface*> monsters_interfaces;
    TowerInterface* tower = nullptr;

    Position<double> tower_map;
    constexpr double limit_angle = (M_PI / 180.0) * 5.0;
    int monsters_created = 0;
    double diff = 0;

    static TimerState state = BOOTING; // State initialization
    static int activ_counter = 0; // Activation counter
    static long tmaxus, tminus;

    struct timeval tcur, tend, tdif;

    int task_period = TASK_PERIOD_MS_TOWER * 1000000;
    rt_task_set_periodic(NULL, TM_NOW, task_period);

    user->add_tower(TowerType::COMPLEX, Position<int>(3, 0));
    while (!terminate_tasks) {
        rt_task_wait_period(NULL);
        world->clear_world_requests();
        auto changes = world->update_world_state();

        for (EntityModification& change : changes) {
            if (change.type_ == EntityType::MONSTER) {
                MonsterInterface* monster_interface = static_cast<MonsterInterface*>(change.entity_);
                monsters_interfaces.push_back(monster_interface);
                monsters_created++;
                if (monsters_created > 10)
                    monsters_created = 1;
            } else if (change.type_ == EntityType::TOWER && tower == nullptr) {
                tower = static_cast<TowerInterface*>(change.entity_);
                tower_map.set_x(tower->get_position().get_x() + 0.5);
                tower_map.set_y(tower->get_position().get_y() + 0.5);
            }
        }

        // Monsters will appear in the first position and the tower will be placed on the top forcing it to
        // rotate to the left, which is the worst case!
        // So they do not need to move!
        //for (MonsterInterface* monster : monsters) {
            //monster->move(MonsterMovement::FRONT);
        //}

        // 10 is the max monsters in the field
        if (tower != nullptr) {
#ifdef WCET
            gettimeofday(&tcur, NULL);
#endif
            // Tower task lifecycle
            std::vector<Position<double>> monsters = tower->radar();
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
                diff = normalize_angle(value - tower->get_angle());

                rt_sem_p(&sem_tower, TM_INFINITE);
#ifdef BLOCKING
                gettimeofday(&tcur, NULL);
#endif
                // Point to the right direction and shoot
                if (diff < -limit_angle)
                    tower->rotate(TowerRotation::RIGHT);
                else if (diff > limit_angle)
                    tower->rotate(TowerRotation::LEFT);

                if (fabs(diff) < M_PI_4)
                    tower->shoot();
#ifdef BLOCKING
                gettimeofday(&tend, NULL);
#endif
                rt_sem_v(&sem_tower);
            }

#ifdef WCET
            gettimeofday(&tend, NULL);
#endif
            timersub(&tend, &tcur, &tdif);
            if (state == BOOTING) {
                activ_counter++;
                if (activ_counter > 10)
                    state = NORMAL;
            } if(state == NORMAL) {
                if (tdif.tv_usec > tmaxus)
                    tmaxus = tdif.tv_usec;
                if (tdif.tv_usec < tminus)
                    tminus = tdif.tv_usec;
                printf("Last instance period = %6ld Maximum = %6ld Minumum = %6ld (us), Seconds: \n", tdif.tv_usec, tmaxus, tminus);
            }

            if (monsters.size() != 0 && diff > limit_angle && fabs(diff) < M_PI_4 && monsters_created >= 10) {
                rt_printf("This is the worst case scenario and also the worst resource usage\n");
            }
        }

        ostringstream stream_serialize;
        world->serialize_data(stream_serialize);
        serialized_string = "MESSAGE" + stream_serialize.str();
        rt_pipe_write(&task_pipe_sender, serialized_string.c_str(), serialized_string.size(), P_NORMAL);
    }
    rt_printf("Tower has finished!\n");
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
    err = rt_task_create(&task_desc, "God Task", TASK_STACK_SIZE, TASK_PRIORITY_TOWER, TASK_MODE);
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
