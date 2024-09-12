
/*
    Members: [S13]
        Shuan Noel Co
        John Marc Gregorio
        Miguel Villanueva
        Angelo Yanto Quiñones

    Current Task Iteration: Week 2 - Group Homework - Setting up your OS emulator
*/

/*
    Imported Built-in C++ Libraries
*/

#include <iostream> // Input & Output
#include <cstdlib> // System Functions
#include <string> // String Functions
#include <map> // mapping
#include <algorithm> // For std::transform
#include <functional> // For std::function
#include <unistd.h> // Sleep

using namespace std;

// Hierarchy of Classes
// 1. Interfaces [GUI]
// 2. Commands [Command Execution]
// 3. SystemOS [System Command Execution]
// 4. ClockCycle [Cycle Simulation]

// Forward Declarations
class Interfaces;
class SystemOS;
class Commands;
class ClockCycle;

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
};

// Command Class Definition

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
            // Manual Exception, for Debugging / Checking Error Hnadling in Clock Cycle
            {"throw-exception", []() { throw runtime_error("OS Error 000: Sample Exception Triggered"); }}
        };

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

// "Cycle Simulation"

class ClockCycle {
    public:
        // Simulate Hardware Clock Cycle
        static void cycle(){

            /*
                [TEMP] INITIAL CYCLE ONLY HAS ONE FEATURE FOR THIS WORK!

                - EXECUTE COMMANDS

                FUTURE CYCLES WILL HAVE MORE BRANCHES AND FEATURES
            */

            // Display Command Input
            Interfaces::displayGetCommand();
            // Execute System Command
            SystemOS::execute("cmd");
        }
        // Simulate Power On
        static void powerOn(){
            // Display OS Hedears & Initial Menu
            Interfaces::displayHeader();
            Interfaces::displayMenu();

            // Main CPU Cycle
            while (true){

                // Error Handling
                try {
                    // Execute One Cycle
                    cycle();
                }
                catch (const std::exception& e){
                    // Display Error Message then Refresh after X seconds
                    std::cerr << e.what() << '\n';
                    cout << "Refreshing in 3 seconds...\n";
                    sleep(3);
                    system("cls");
                    Interfaces::displayHeader();
                    Interfaces::displayMenu();
                }
            }
        }   
};


// Emulator
int main()
{
    // Execute Emulated CSOPESY OS
    ClockCycle::powerOn(); 
    return 0;
}