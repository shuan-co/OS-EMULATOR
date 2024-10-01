
/*
    Members: [S13]
        Shuan Noel Co
        John Marc Gregorio
        Miguel Villanueva
        Angelo Yanto Quiñones

    Current Task Iteration: Week 2 - Group Homework - Setting up your OS emulator
*/

#define UNICODE
#define _UNICODE

using namespace std;

// Hierarchy of Classes
// 1. Interfaces [GUI]
// 2. Commands [Command Execution]
// 3. SystemOS [System Command Execution]
// 4. ClockCycle [Cycle Simulation]

// Operating System Libraries
#include "interfaces.h"
#include "commands.h"
#include "systemos.h"
#include "clockcycle.h"
#include "process.h"
#include "current_state.h"

// Emulator


int main()
{
    // Execute Emulated CSOPESY OS
    ClockCycle::powerOn();
    return 0;
}   
