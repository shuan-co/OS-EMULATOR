#ifndef SYSTEMOS_H
#define SYSTEMOS_H

// Libraries

#include <string.h>
#include <algorithm>

// Operating System Libraries

#include "interfaces.h"
#include "commands.h"

using namespace std;

/*
    All System Functionalities and Commands
*/

// System Class Definition

class SystemOS
{
private:
    // Get Terminal Syntax Input
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
    static void execute(const string &type)
    {
        // Execute System Commands
        if (type == "cmd")
        {

            // Display, Get & Execute Command
            Interfaces::displayGetCommand();
            Commands::execute(getInput());
        }
    }
};

#endif