#ifndef COMMANDS_H
#define COMMANDS_H

// Libraries

#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <functional>
#include <unistd.h>
#include <windows.h>
#include <thread>
#include <conio.h>

// Operating System Libraries

#include "interfaces.h"

using namespace std;

/*
    All Command Syntaxes & Functionalities
*/

class Commands
{
public:
    //  Implemented Functions

    static void clear()
    {
        system("cls");
        Interfaces::displayHeader();
        Interfaces::displayMenu();
    }

    static void exit()
    {
        std::exit(0);
    }

    // Template Functions: TODO [Command Executions]

    static void initialize()
    {
        cout << "initialize command recognized. Doing Something...\n";
        // DO SOMETHING HERE
    }

    static void screen()
    {
        cout << "screen command recognized. Doing Something...\n";
        // DO SOMETHING HERE
    }

    static void schedulerTest()
    {
        cout << "scheduler-test command recognized. Doing Something...\n";
        // DO SOMETHING HERE
    }

    static void schedulerStop()
    {
        cout << "scheduler-stop command recognized. Doing Something...\n";
        // DO SOMETHING HERE
    }

    static void reportUtil()
    {
        cout << "report-util command recognized. Doing Something...\n";
        // DO SOMETHING HERE
    }

    // Marquee Console Simulator
    static void marqueeConsole()
    {
        bool isExit = false;
        string userInput;

        while (!isExit)
        {
            // Load to X
            for (int i = 1; i <= 10; i++)
            {
                Interfaces::printLoadingBar(i);
                cout << "Enter a command [Q Enter: Exit]: " << userInput << flush;

                if (_kbhit())
                {
                    char ch = _getch(); 

                    if (ch == 8) 
                    {
                        if (!userInput.empty())
                        {
                            userInput.pop_back();
                            cout << "\b \b";
                        }
                    }
                    else if (ch == 13)
                    {
                        if (userInput == "q" || userInput == "Q")
                        {
                            isExit = true;
                            break;
                        }
                    }
                    else
                    {
                        userInput += ch;
                        cout << ch;
                    }
                }
                Sleep(1);
            }

            if (isExit)
            {
                system("cls");
                Interfaces::displayHeader();
                Interfaces::displayMenu();
                break;
            }
        }
    }

    // Executer

    static void execute(const string &command)
    {
        /*
            Create Association Mapping between string command syntax and static void function executions
        */
        static const std::map<std::string, std::function<void()>> commandMap = {
            {"clear", Commands::clear},
            {"exit", Commands::exit},
            {"initialize", Commands::initialize},
            {"screen", Commands::screen},
            {"scheduler-test", Commands::schedulerTest},
            {"scheduler-stop", Commands::schedulerStop},
            {"report-util", Commands::reportUtil},
            // Marque Console Sampler
            {"marquee-console", Commands::marqueeConsole},
            // Manual Exception, for Debugging / Checking Error Hnadling in Clock Cycle
            {"throw-exception", []()
             { throw runtime_error("OS Error 000: Sample Exception Triggered"); }}};

        /*
            Check if command key can be found in the mapping
             - If found, find the respective value pair of the key command and execute it
             - Else, print error message
        */
        if (commandMap.count(command))
        {
            commandMap.at(command)();
        }
        else
        {
            cout << "Command not recognized. Please try again.\n";
        }
    }
};

#endif