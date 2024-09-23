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
#include "process.h"
#include "current_state.h"


using namespace std;

/*
    All Command Syntaxes & Functionalities
*/

class Commands
{

private:
    static ProcessManager processManager;

public:
    //  Implemented Functions

    static void clear (const std::string& args, ProgramState& state)
    {
        system("cls");
        Interfaces::displayHeader();
        Interfaces::displayMenu();
    }

    static void exit(const std::string& args, ProgramState& state)
    {
        std::exit(0);
    }

    // Template Functions: TODO [Command Executions]

    static void initialize(const std::string& args, ProgramState& state)
    {
        cout << "initialize command recognized. Doing Something...\n";
        // DO SOMETHING HERE
    }

    static void screen(const std::string& args, ProgramState& programState) {
        std::istringstream iss(args);
        std::string option, name;
        iss >> option >> name;

        if (option == "-s") {
            bool created = processManager.createProcess(name);
            system("cls");
            if (created) {
                std::cout << "Created screen for process: " << name << std::endl;
                processManager.displayProcess(name);
                programState.setContext(Context::PROCESS_SCREEN);
                programState.setCurrentProcess(name);
            }
            else {
                system("cls");
                std::cout << "Process already exists\n";
                Interfaces::displayHeader();
                Interfaces::displayMenu();
            }
        } else if (option == "-r") {
            system("cls");
            processManager.displayProcess(name);
            programState.setContext(Context::PROCESS_SCREEN);
            programState.setCurrentProcess(name);
        }
        else {
            std::cout << "Invalid screen command. Use -s to create or -r to display." << std::endl;
        }
    }

    static void schedulerTest(const std::string& args, ProgramState& state)
    {
        cout << "scheduler-test command recognized. Doing Something...\n";
        // DO SOMETHING HERE
    }

    static void schedulerStop(const std::string& args, ProgramState& state)
    {
        cout << "scheduler-stop command recognized. Doing Something...\n";
        // DO SOMETHING HERE
    }

    static void reportUtil(const std::string& args, ProgramState& state)
    {
        cout << "report-util command recognized. Doing Something...\n";
        // DO SOMETHING HERE
    }

    // Marquee Console Simulator
    static void marqueeConsole(const std::string& args, ProgramState& state)
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

    static void execute(const string& command, ProgramState& programState)
    {
        /*
            Create Association Mapping between string command syntax and static void function executions
        */
        static const std::map<std::string, std::function<void(const string&, ProgramState&)>> commandMap = {
            {"clear", Commands::clear},
            {"exit", Commands::exit},
            {"initialize", Commands::initialize},
            {"screen", Commands::screen},
            {"scheduler-test", Commands::schedulerTest},
            {"scheduler-stop", Commands::schedulerStop},
            {"report-util", Commands::reportUtil},
            // Marquee Console Sampler
            {"marquee-console", Commands::marqueeConsole},
            // Manual Exception, for Debugging / Checking Error Handling in Clock Cycle
            {"throw-exception", [](const string&, ProgramState&)
             {
                 throw runtime_error("OS Error 000: Sample Exception Triggered");
             }} };

        /*
            Check if command key can be found in the mapping
             - If found, find the respective value pair of the key command and execute it
             - Else, print error message
        */

        std::istringstream iss(command);
        std::string cmd, args;
        iss >> cmd;
        std::getline(iss >> std::ws, args);

        if (commandMap.count(cmd))
        {
            commandMap.at(cmd)(args, programState);
        }
        else
        {
            cout << "Command not recognized. Please try again.\n";
        }
    }
};

ProcessManager Commands::processManager;

#endif