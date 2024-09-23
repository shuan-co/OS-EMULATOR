#ifndef SYSTEMOS_H
#define SYSTEMOS_H

// Libraries

#include <string.h>
#include <algorithm>

// Operating System Libraries

#include "interfaces.h"
#include "commands.h"
#include "current_state.h"

using namespace std;

/*
    All System Functionalities and Commands
*/

// System Class Definition

class SystemOS
{
private:
    // Get Terminal Syntax Input
    static ProgramState programState;

    static string getInput()
    {
        string inputSyntax;

        // Get entire line of input
        std::getline(cin, inputSyntax);

        // Lowercase
        transform(inputSyntax.begin(), inputSyntax.end(), inputSyntax.begin(), ::tolower);

        return inputSyntax;
    }

public:
    static bool execute(const string& type) {
        if (type == "cmd") {
            string input = getInput();

            if (input == "exit") {
                if (programState.getContext() == Context::PROCESS_SCREEN) {
                    // Galing process to menu
                    programState.setContext(Context::MAIN_MENU);
                    programState.setCurrentProcess("");
                    system("cls");
                    Interfaces::displayHeader();
                    Interfaces::displayMenu();
                    return true;
                }
                else {
                    // Galing menu to terminate
                    return false;
                }
            }

            // Pinagbawalan ko user to use any other command besides exit medyo hard coded pa siya
            if (programState.getContext() == Context::PROCESS_SCREEN) {
                if (input == "clear" || input == "initialize" || input == "screen" ||
                    input == "scheduler-test" || input == "scheduler-stop" ||
                    input == "report-util" || input == "marquee-console" ||
                    input == "throw-exception") {
                    std::cout << "Command not recognized. Please try again.\n";
                    return true; 
                }
            }

            Commands::execute(input, programState);
            return true;
        }
        return true;
    }

    static ProgramState& getProgramState() {
        return programState;
    }
};

ProgramState SystemOS::programState;

#endif