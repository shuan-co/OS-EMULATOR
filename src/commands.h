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
#include <array>
#include <thread>

// Operating System Libraries

#include "interfaces.h"
#include "process.h"
#include "current_state.h"
#include "dummyProcess.h"


using namespace std;

/*
    All Command Syntaxes & Functionalities
*/

class Commands
{

private:
    static ProcessManager processManager;

    static string currentUserInputCommand;
    static bool isExit;
    static array<string, 3> commandHistory;
    static int historyIndex;

    static void storeCommandInHistory(const string &command) {
        commandHistory[historyIndex] = command; 
        historyIndex = (historyIndex + 1) % 3; 
    }
    
    static bool keyboardPolling() {
        if (_kbhit()) {
            char ch = _getch();

            if (ch == '\r') {
                std::cout << "\n"; 

                if (currentUserInputCommand == "exit") {
                    currentUserInputCommand.clear(); 
                    return true;                     
                }

                if (!currentUserInputCommand.empty()) {
                    storeCommandInHistory(currentUserInputCommand);
                    currentUserInputCommand.clear(); 
                }
            }
            else if (ch == '\b') {
                if (!currentUserInputCommand.empty()) {
                    currentUserInputCommand.pop_back(); 
                    std::cout << "\b \b";              
                }
            }
            else if (isprint(ch)) {
                currentUserInputCommand += ch;
            }
        }
        return false; 
    }

    static void runMarqueeAnimation(const std::string &marqueeText, int &x, int &y, int currentWidth, int currentHeight)
    {
        bool movingDown = true;
        bool movingRight = true;

        while (!isExit)
        {
            Sleep(60); 
            Interfaces::marqueeConsoleAnimation(x, y, marqueeText);
            Interfaces::displayUserGetCommand(Commands::currentUserInputCommand, currentHeight - 6);
            Interfaces::displayCommandHistory(Commands::commandHistory, Commands::historyIndex, currentHeight - 6);
            Interfaces::resetCursorPositionMarquee(Commands::currentUserInputCommand, currentHeight - 6);
            
            if (movingDown) {
                if (y < currentHeight - 10) {
                    y++; 
                } else {
                    movingDown = false; 
                    y--;
                }
            } else {
                if (y > 0) {
                    y--; 
                }
                else {
                    movingDown = true;
                    y++;
                }
            }

            if (movingRight) {
                if (x < currentWidth - marqueeText.length() - 1) {
                    x++; 
                }
                else {
                    movingRight = false;
                }
            } else {
                if (x > 0) {
                    x--; 
                } else {
                    movingRight = true; 
                }
            }
        }
    }

    static void runKeyboardPolling()
    {
        while (!isExit)
        {
            Sleep(15); // Polling Rate
            isExit = Commands::keyboardPolling();
        }
    }

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

    // CSOPESY-SMI [NVIDIA-SMI]
    static void opesyosSMI(const std::string &args, ProgramState &state)
    {

        // Dummy Process Data
        system("cls");
        DummyProcess stack[5];
        string processNames[5] = {"nt.CBS_cw5n1h2txyewy\\SearchHost.exe",
                                  "C:\\Program Files (x86)\\steamapps\\common\\wallpaper_engine\\wallpaper64.exe",
                                  "C:\\Program Files\\NVIDIA Corporation\\GeForce Experience\\NVIDIA Share.exe",
                                  "C:\\Program Files (x86)\\Steam\\bin\\cef\\cef.win7x64\\steamwebhelper.exe",
                                  "C:\\Users\\User\\AppData\\Local\\Microsoft\\Launcher\\VerylongVerylongVerylongVerylongVerylong.PowerLauncher.exe"};
        for (int i = 0; i < 5; i++)
        {
            stack[i].gpu = i;
            stack[i].gi = i + 10;    
            stack[i].ci = i + 20;    
            stack[i].pid = 1000 + i;
            stack[i].type = "C+G";
            stack[i].processName = processNames[i]; 
            stack[i].gpuMemory = "N/A";            
        }

        // Display 
        Interfaces::displaySMIHeader();
        Interfaces::displayProcessInfo(5, stack);

        // Exit

        std::cin.get();

        system("cls");
        Interfaces::displayHeader();
        Interfaces::displayMenu();
    }

    // Marquee Console Simulator
    static void marqueeConsole(const std::string &args, ProgramState &state)
    {
        isExit = false; 
        const std::string marqueeText = "Hello world in marquee!";
        int currentWidth, currentHeight;
        Interfaces::GetConsoleSize(currentWidth, currentHeight);

        int y = 0;
        int x = 0;

        std::thread marqueeThread(runMarqueeAnimation, 
                std::ref(marqueeText), std::ref(x), 
                std::ref(y), currentWidth, currentHeight);
        std::thread pollingThread(runKeyboardPolling);

        marqueeThread.join();
        pollingThread.join();

        Commands::currentUserInputCommand.clear();
        system("cls");
        Interfaces::displayHeader();
        Interfaces::displayMenu();
    }

    // Executer
    static void execute(const string& command, ProgramState& programState)
    {
        /*
            Create Association Mapping between string command syntax and static void function executions
        */
        static const std::map<std::string, std::function<void(const string &, ProgramState &)>> commandMap = {
            {"clear", Commands::clear},
            {"exit", Commands::exit},
            {"initialize", Commands::initialize},
            {"screen", Commands::screen},
            {"scheduler-test", Commands::schedulerTest},
            {"scheduler-stop", Commands::schedulerStop},
            {"report-util", Commands::reportUtil},
            {"opesyos-smi", Commands::opesyosSMI},
            // OPESYOS SMI [NVIDIA-SMI]
            // Marquee Console Sampler
            {"marquee-console", Commands::marqueeConsole},
            // Manual Exception, for Debugging / Checking Error Handling in Clock Cycle
            {"throw-exception", [](const string &, ProgramState &)
             {
                 throw runtime_error("OS Error 000: Sample Exception Triggered");
             }}};

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
string Commands::currentUserInputCommand = "";
bool Commands::isExit = false;

// Initialize command history array and index
std::array<std::string, 3> Commands::commandHistory = {"", "", ""};
int Commands::historyIndex = 0;
#endif