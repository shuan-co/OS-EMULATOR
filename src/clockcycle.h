#ifndef CLOCKCYCLE_H
#define CLOCKCYCLE_H

// Libraries
#include <iostream>
#include <unistd.h>
#include <windows.h>

// Operating System Libraries
#include "interfaces.h"
#include "systemos.h"

using namespace std;

// "Cycle Simulation"
class ClockCycle
{
public:
    // Simulate Hardware Clock Cycle
    static bool cycle()
    {
        /*
            [TEMP] INITIAL CYCLE ONLY HAS ONE FEATURE FOR THIS WORK!
            - EXECUTE COMMANDS
            FUTURE CYCLES WILL HAVE MORE BRANCHES AND FEATURES
        */
        // Display Command Input
        Interfaces::displayGetCommand();
        // Execute System Command
        return SystemOS::execute("cmd");
    }

    // Simulate Power On
    static void powerOn()
    {
        // Display OS Headers & Initial Menu
        Interfaces::displayHeader();
        Interfaces::displayMenu();

        // Main CPU Cycle
        while (true)
        {
            // Error Handling
            try
            {
                // Execute One Cycle
                if (!cycle()) {
                    // Exit command received in main menu
                    break;
                }
            }
            catch (const std::exception& e)
            {
                // Display Error Message then Refresh after X seconds
                std::cerr << e.what() << '\n';
                cout << "Refreshing in 3 seconds...\n";
                Sleep(3000);
                system("cls");
                Interfaces::displayHeader();
                Interfaces::displayMenu();
            }
        }

        // Exit message
        cout << "Shutting down the system. Goodbye!" << endl;
    }
};

#endif