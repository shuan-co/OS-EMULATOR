#ifndef CLOCKCYCLE_H
#define CLOCKCYCLE_H

// Libraries
#include <iostream>
#include <unistd.h>
#include <windows.h>

// Operating System Libraries
#include "interfaces.h"
#include "process.h"
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
        int numCpu = getCPUnum("config.txt");

        // Display OS Headers & Initial Menu

        Interfaces::displayHeader();
        Interfaces::displayMenu();

        // Main CPU Cycle
        while (true)
        {
            FCFSScheduler::incrementIdleTicks(numCpu);
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

    static int getCPUnum(const std::string& filePath)
    {
        std::ifstream configFile(filePath);
        if (!configFile.is_open())
        {
            std::cerr << "Error: Unable to open configuration file: " << filePath << "\n";
            return 1;
        }

        std::string line;
        while (std::getline(configFile, line))
        {
            std::istringstream iss(line);
            std::string key;
            int value;

            iss >> key >> value;
            if (key == "num-cpu")
            {
                return value;
            }
        }

        std::cerr << "Error: 'num-cpu' not found in configuration file.\n";
        return 1;
    }

};

#endif