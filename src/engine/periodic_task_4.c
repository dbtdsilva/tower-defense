/* ************************************************************
* Xenomai - creates two periodic tasks
*           with different priorities and loads
*         - tasks share the same code and receive as input args
*           the period
*
* Paulo Pedreiras, Jul/2013
* 
* 
************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <sys/mman.h> // For mlockall
#include <sys/types.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/pipe.h>

#include <rtdk.h> // Provides rt_print functions

#define TASK_MODE 0  	// No flags
#define TASK_STKSZ 0 	// Default stack size

#define TASK_A_PRIO 99 	// RT priority [0..99]
#define TASK_A_PERIOD_NS 1000000000 // Task period, in ns

#define TASK_LOAD_NS 10000000 // Task execution time, in ns (same to all tasks)

RT_TASK task_a_desc; // Task decriptor
RT_PIPE task_pipe;

/*
* Catches CTRL + C to allow a controlled termination of the application
*/
void catch_signal(int sig) {}

void wait_for_ctrl_c(void) {
	signal(SIGTERM, catch_signal); //catch_signal is called if SIGTERM received
	signal(SIGINT, catch_signal);  //catch_signal is called if SIGINT received

	// Wait for CTRL+C or sigterm
	pause();
	
	// Will terminate
	rt_printf("Terminating ...\n");
}

/*
* Simulates the computational load of tasks
*/ 
void simulate_load(RTIME load_ns) {
	RTIME ti, tf;
	
	ti = rt_timer_read(); // Get initial time
	tf = ti + load_ns;      // Compute end time
	while(rt_timer_read() < tf); // Busy wait

	return;
}

/*
* Task body implementation
*/
void task_code(void *task_period_ns) {
	int len_hello, len_goodbye, err;
	int *task_period;

	len_hello = sizeof("Hello") + 1;
	len_goodbye = sizeof("Goodbye") + 1;
	task_period = (int *) task_period_ns;

	err = rt_task_set_periodic(NULL, TM_NOW, *task_period);
	for(;;) {
		err = rt_task_wait_period(NULL);

		if(err) {
			rt_printf("Error on waiting for period (error code = %d)\n", err);
			return;
		}

		rt_printf("Preparing to send Hello\n");
		err = rt_pipe_write(&task_pipe, "Hello", len_hello, P_NORMAL);

		if(err < 0) {
			rt_printf("Error sending Hello message (error code = %d)\n", err);
			return;
		} else
			rt_printf("Hello message sent successfully\n");

		simulate_load(TASK_LOAD_NS);

		rt_printf("Preparing to send Goodbyee\n");
		err = rt_pipe_write(&task_pipe, "Goodbye", len_goodbye, P_NORMAL);

		if(err < 0) {
			rt_printf("Error sending Goodbye message (error code = %d)\n", err);
			return;
		} else
			rt_printf("Goodbye message sent successfully\n");
	}
}


/*
* Main function
*/ 
int main(int argc, char *argv[]) {
	int err, task_a_period_ns = TASK_A_PERIOD_NS;

	/* Perform auto-init of rt_print buffers if the task doesn't do so */
	rt_print_auto_init(1);

	/* Lock memory to prevent paging */
	mlockall(MCL_CURRENT | MCL_FUTURE);

	err = rt_pipe_create(&task_pipe, NULL, 0, 0);

	if(err) {
		rt_printf("Error creating pipe (error code = %d)\n", err);
		return err;
	} else
		rt_printf("Pipe created successfully\n");

	/* Create RT task */
	/* Args: descriptor, name, stack size, prioritry [0..99] and mode (flags for CPU, FPU, joinable ...) */
	err = rt_task_create(&task_a_desc, "Task a", TASK_STKSZ, TASK_A_PRIO, TASK_MODE);
	
	if(err) {
		rt_printf("Error creating task a (error code = %d)\n", err);
		return err;
	} else 
		rt_printf("Task a created successfully\n");
		
	/* Start RT task */
	/* Args: task decriptor, address of function/implementation and argument*/
	rt_task_start(&task_a_desc, &task_code, (void *) &task_a_period_ns);

	/* wait for termination signal */	
	wait_for_ctrl_c();

	return 0;
}
