#ifndef SYSTEMOS_H
#define SYSTEMOS_H

// Libraries
#include <string.h>
#include <algorithm>
#include <conio.h>
#include <iostream>
#include <array>

// Operating System Libraries
#include "interfaces.h"
#include "current_state.h"

using namespace std;

/*
    All System Functionalities and Commands
*/

// System Class Definition

class Commands;

class SystemOS
{
private:
    static ProgramState programState;

    static string getInput()
    {
        string inputSyntax;
        std::getline(cin, inputSyntax);

        // Convert to lowercase
        transform(inputSyntax.begin(), inputSyntax.end(), inputSyntax.begin(), ::tolower);
        return inputSyntax;
    }


public:
    static bool execute(const string &type)
    {
        if (type == "cmd")
        {
            string input = getInput();

            if (input == "exit")
            {
                if (programState.getContext() == Context::PROCESS_SCREEN)
                {
                    programState.setContext(Context::MAIN_MENU);
                    programState.setCurrentProcess("");
                    system("cls");
                    Interfaces::displayHeader();
                    Interfaces::displayMenu();
                    return true;
                }
                else
                {
                    return false;
                }
            }

            if (programState.getContext() == Context::PROCESS_SCREEN)
            {
                if (input == "clear" || input == "initialize" || input == "screen" ||
                    input == "scheduler-test" || input == "scheduler-stop" ||
                    input == "report-util" || input == "marquee-console" ||
                    input == "throw-exception" || input == "opesyos-smi")
                {
                    std::cout << "Command not recognized. Please try again.\n";
                    return true;
                }
            }

            Commands::execute(input, programState);
            return true;
        }
        return true;
    }

    static ProgramState &getProgramState()
    {
        return programState;
    }

};

// Initialize static members outside the class
ProgramState SystemOS::programState;

#endif
