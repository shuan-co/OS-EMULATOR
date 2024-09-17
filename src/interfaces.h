#ifndef INTERFACES_H 
#define INTERFACES_H

// Libraries

#include <iostream>

using namespace std;

/*
    All Graphical User Interface Designs and Animations
*/


// GUI Interfaces Class Definition

class Interfaces
{
public:
    // Main OS Header Logo
    static void displayHeader()
    {
        cout << " ______     ______     ______     ______   ______     ______     __  __    \n";
        cout << "/\\  ___\\   /\\  ___\\   /\\  __ \\   /\\  == \\ /\\  ___\\   /\\  ___\\   /\\ \\_\\ \\   \n";
        cout << "\\ \\ \\____  \\ \\___  \\  \\ \\ \\/\\ \\  \\ \\  _-/ \\ \\  __\\   \\ \\___  \\  \\ \\____ \\  \n";
        cout << " \\ \\_____\\  \\/\\_____\\  \\ \\_____\\  \\ \\_\\    \\ \\_____\\  \\/\\_____\\  \\/\\_____\\ \n";
        cout << "  \\/_____/   \\/_____/   \\/_____/   \\/_/     \\/_____/   \\/_____/   \\/_____/ \n";
        cout << "=============================================================================\n";
    }

    // Main OS Menu
    static void displayMenu()
    {
        cout << "\033[38;2;21;188;12m"; // RGB(21, 188, 12)
        cout << "Hello, Welcome to CSOPESY Command Line!\n";

        cout << "\033[38;2;226;219;150m"; // RGB(226, 219, 150)
        cout << "Type 'exit' to quit, 'clear' to clear the screen\n";
    }

    // Display Command Input
    static void displayGetCommand()
    {
        // Reset to default color
        cout << "\033[0m";
        cout << "Enter a command: ";
    }

    // Loading Bar Animation [Marquee Console]
    static void printLoadingBar(int length)
    {
        system("cls"); // Clear the console

        cout << "["; // Start of the loading bar
        for (int i = 0; i < length; i++)
        {
            cout << "[]"; // Print the filled part
        }
        for (int i = length; i < 10; i++)
        {                 // Assuming max size of 10 for the loading bar
            cout << "  "; // Print the unfilled part
        }
        cout << "]" << endl; // End of the loading bar
    }
};

#endif