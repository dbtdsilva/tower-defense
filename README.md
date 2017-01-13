# Tower Defense

This project came across with the idea of developing a faster game to implement using real-time techniques. So it was decided to implement Tower Defense as real-time system.
The Tower Defense game is composed of a path, where monsters have to find their way to the end. The objective of the game is to not allow the monsters to reach the end of the path. To do so a human player must place towers alongside the path, that will try to take out the monsters while they traverse it. The human player can place or remove towers, but it can't interfere with the behaviour of the towers. To make the game more interesting, difficulty will increase when the player surpasses a level. This increase in the difficulty was implemented by introducing a stronger type of monster.

## System description
### Real-time constraints

Since the game was implemented as real-time system, there was the need to fulfill real-time requirements. To fulfill this requirements, the game architecture had to be thought out with the need to use Xenomai, since it provides the capabilities to work in the real-time mode.

A game usually works with a main cycle that checks the player input, processes game events (like moving objects) and then renders the new frame.

However, Xenomai works with tasks, which can be periodic, aperiodic or sporadic. Therefore, we had to adapt this architecture to use the tasks in a meaningful way according to their characteristics:
* Periodic tasks - Tasks are activated periodically according to the time period.
* Aperiodic tasks - The times are not known, so we cannot predict their behaviour. The only values we can work with are statistics.
* Sporadic tasks - Tasks are activated when events occur. It has a minimum interval time (MIT) for new occurrence which means that a new activation occurs after at least MIT. Those kind of tasks are a subgroup of aperiodic tasks.

### System tasks

Our implemented has the following tasks.

Periodic tasks:
* God - It is responsible for processing all the requests from the other tasks, ensuring that the laws of the simulated world are not violated, creation and removal of tasks and sending the world state information to the viewer.
* Tower - It is responsible for checking the information that its radar sees, processing that information and then issue a request to rotate the cannon and shoot. Each tower has its own task associated.
* Monster - It is responsible for checking the information that it receives from the eyes, processing that information and then issue a request to move and rotate. Each monster has its own task.

Sporadic tasks:
* User interaction - It is responsible for receiving the user interaction from the viewer and issue a request to the god task to apply that interaction.

### More details

You can find more details about this project in the report. 
https://github.com/dbtdsilva/tower-defense/blob/master/docs/report.pdf

## How to run this project

### Requirements

This project has some requirements:
* Xenomai 2.6.5 installed and patched with the kernel
* SDL2 and SDL2_image

It also uses Cereal but it will download it using CMake.

### Cloning and compiling the code

```
git clone https://github.com/dbtdsilva/tower-defense.git
mkdir tower-defense/build
cd tower-defense/build
cmake ..
make
```

### Running

There are 6 executables within this project, but you will only need 2 of them to see the Tower Defense working:
* towerdefense - This implements the core of the game and the tasks as well, so it will run on the kernel space;
* towerdefense_viewer - This implements a viewer that reads info sent through a pipe by the engine and draws it using SDL2 (in user space).

The other 4 executables were used to take some measures to make sure that the system was schedulable.
