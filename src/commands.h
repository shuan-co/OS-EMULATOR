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
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>


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
    static std::thread schedulerThread;
    static bool isSchedulerRunning;

    static void storeCommandInHistory(const string& command) {
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

    static void runMarqueeAnimation(const std::string& marqueeText, int& x, int& y, int currentWidth, int currentHeight)
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
                }
                else {
                    movingDown = false;
                    y--;
                }
            }
            else {
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
            }
            else {
                if (x > 0) {
                    x--;
                }
                else {
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

    static void runDebugSchedulerTest()
    {
        int i = 0;
        int iteration = 0;
        while (i < 10)
        {
            if (iteration % configSettings.batchProcessFreq == 0)
            {
                processManager.createProcess("process" + std::to_string(i), configSettings.minIns, configSettings.maxIns);
                i++;
            }

            iteration++;
            Sleep(500);
        }
    }

    static void runSchedulerTest()
    {
        int i = 0;
        int iteration = 0;
        while (Commands::isSchedulerRunning)
        {
            if (iteration % configSettings.batchProcessFreq == 0)
            {
                processManager.createProcess("process" + std::to_string(i), configSettings.minIns, configSettings.maxIns);
                i++;
            }

            iteration++;
            Sleep(500);
        }
    }

    static bool compareProcesses(const DummyProcess &a, const DummyProcess &b)
    {
        return a.pid < b.pid; // Sort in descending order of gi
    }

    static string trim(const std::string &str)
    {
        size_t start = str.find_first_not_of(' ');

        if (start == std::string::npos)
            return "";

        size_t end = str.find_last_not_of(' ');

        return str.substr(start, end - start + 1);
    }

    static bool isStringEmptyOrWhitespace(const std::string &str)
    {
        std::string trimmedStr = trim(str);
        return trimmedStr.empty();
    }

public:

    struct ConfigSettings {
        int numCpu;
        std::string scheduler;
        int quantumCycles;
        int batchProcessFreq;
        int minIns;
        int maxIns;
        int delaysPerExec;
    };

    static ConfigSettings configSettings;


    //  Implemented Functions
    static void clear(const std::string& args, ProgramState& state)
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
        std::ifstream configFile("config.txt");
        if (!configFile.is_open()) {
            std::cout << "Error: Unable to open config.txt\n";
            return;
        }

        std::string line;
        while (std::getline(configFile, line)) {
            std::istringstream iss(line);
            std::string key;
            iss >> key;

            if (key == "num-cpu") {
                iss >> configSettings.numCpu;
                if (configSettings.numCpu < 1 || configSettings.numCpu > 128) {
                    std::cout << "Error: num-cpu must be between 1 and 128\n";
                    return;
                }
                FCFSScheduler::setCPUThreads(configSettings.numCpu);
            } else if (key == "scheduler") {
                iss >> configSettings.scheduler;
                if (configSettings.scheduler != "fcfs" && configSettings.scheduler != "rr") {
                    std::cout << "Error: scheduler must be 'fcfs' or 'rr'\n";
                    return;
                }
                if (configSettings.scheduler == "rr"){
                    FCFSScheduler::setRoundRobin(true);
                }
            } else if (key == "quantum-cycles") {
                iss >> configSettings.quantumCycles;
                if (configSettings.quantumCycles < 1 || configSettings.quantumCycles > std::numeric_limits<int>::max()) {
                    std::cout << "Error: quantum-cycles must be between 1 and 2^32\n";
                    return;
                }
                ProcessQueue::setQuantumSplice(configSettings.quantumCycles);
            } else if (key == "batch-process-freq") {
                iss >> configSettings.batchProcessFreq;
                if (configSettings.batchProcessFreq < 1 || configSettings.batchProcessFreq > std::numeric_limits<int>::max()) {
                    std::cout << "Error: batch-process-freq must be between 1 and 2^32\n";
                    return;
                }
            } else if (key == "min-ins") {
                iss >> configSettings.minIns;
                if (configSettings.minIns < 1 || configSettings.minIns > std::numeric_limits<int>::max()) {
                    std::cout << "Error: min-ins must be between 1 and 2^32\n";
                    return;
                }
            } else if (key == "max-ins") {
                iss >> configSettings.maxIns;
                if (configSettings.maxIns < 1 || configSettings.maxIns > std::numeric_limits<int>::max()) {
                    std::cout << "Error: max-ins must be between 1 and 2^32\n";
                    return;
                }
            } else if (key == "delays-per-exec") {
                iss >> configSettings.delaysPerExec;
                if (configSettings.delaysPerExec < 0 || configSettings.delaysPerExec > std::numeric_limits<int>::max()) {
                    std::cout << "Error: delays-per-exec must be between 0 and 2^32\n";
                    return;
                }
                FCFSScheduler::setDelayPerExec(configSettings.delaysPerExec);
            }
        }

        configFile.close();
        std::cout << "Configuration initialized successfully.\n";
        state.setInitialized(true);
    }

    static void screen(const std::string& args, ProgramState& programState) {
        std::istringstream iss(args);
        std::string option, name;
        iss >> option >> name;

        if (isStringEmptyOrWhitespace(name) && option != "-ls")
        {
            std::cout << "Invalid screen command. Please provide a process name.\n";
            return;
        }

        if (option == "-s")
            {
                bool created = processManager.createProcess(name, configSettings.minIns, configSettings.maxIns);
                if (created)
                {
                    system("cls");
                    std::cout << "Created screen for process: " << name << std::endl;
                    processManager.displayProcess(name);
                    programState.setContext(Context::PROCESS_SCREEN);
                    programState.setCurrentProcess(name);
                }
                else
                {
                    std::cout << "Process already exists\n";
                }
            }
            else if (option == "-r")
            {
                system("cls");
                processManager.displayProcess(name);
                programState.setContext(Context::PROCESS_SCREEN);
                programState.setCurrentProcess(name);
            }
            else if (option == "-ls")
            {
                opesyosSMI();
            }
            else
            {
                std::cout << "Invalid screen command. Use -s to create, -r to display or -ls to view all processes" << std::endl;
            }
        }

    static void schedulerTest(const std::string &args, ProgramState &state)
    {
        if (isSchedulerRunning)
        {
            std::cout << "Scheduler is already running." << std::endl;
            return;
        }

        isSchedulerRunning = true;
        schedulerThread = std::thread(runSchedulerTest);
        std::cout << "Scheduler starting..." << std::endl;
    }

    static void schedulerStop(const std::string &args, ProgramState &state)
    {
        if (!isSchedulerRunning)
        {
            std::cout << "Scheduler is not running." << std::endl;
            return;
        }

        std::cout << "Stopping the scheduler..." << std::endl;

        isSchedulerRunning = false;

        if (schedulerThread.joinable())
        {
            schedulerThread.join(); 
        }

        std::cout << "Scheduler stopped successfully." << std::endl;
    }

    static void debugSchedulerTest(const std::string& args, ProgramState& state)
    {
        if (isSchedulerRunning)
        {
            std::cout << "Scheduler is already running." << std::endl;
            return;
        }

        isSchedulerRunning = true;
        schedulerThread = std::thread(runDebugSchedulerTest);
        std::cout << "Scheduler started successfully." << std::endl;
    }

    static void reportUtil(const std::string& args, ProgramState& state)
    {
        // Open the file in append mode
        std::ofstream logFile("./csopesy-log.txt", std::ios::app);
        if (!logFile.is_open())
        {
            std::cerr << "Error: Unable to open log file." << std::endl;
            return;
        }

        // Get the list of processes from the ProcessManager
        std::unordered_map<std::string, Process> processes = processManager.getAllProcesses();

        if (processes.empty())
        {
            logFile << "No processes are currently running." << std::endl;
        }
        else
        {
            logFile << "----------------------------------\n";
            logFile << "CPU utilization: "
                    << (static_cast<double>(FCFSScheduler::getRunningWorkersCount()) / FCFSScheduler::getCPUThreads()) * 100
                    << "%" << std::endl;
            logFile << "Cores used: " << FCFSScheduler::getRunningWorkersCount() << std::endl;
            logFile << "Cores available: " << FCFSScheduler::getCPUThreads() - FCFSScheduler::getRunningWorkersCount() << "\n"
                    << std::endl;

            std::vector<DummyProcess> realProcesses;
            int gpuId = 0;

            for (const auto &pair : processes)
            {
                const Process &proc = pair.second;
                DummyProcess dp;
                dp.gpu = gpuId++;
                dp.gi = proc.currentLine;
                dp.ci = proc.totalLines;
                dp.pid = proc.pid;
                dp.type = proc.getTimestamp();
                dp.processName = proc.name;
                dp.gpuMemory = "N/A";
                dp.core = proc.cpu;

                realProcesses.push_back(dp);
            }
            std::sort(realProcesses.begin(), realProcesses.end(), compareProcesses);

            logFile << "Running Processes:\n";
            for (const auto &proc : realProcesses)
            {
                if (proc.gi < proc.ci)
                {
                    logFile << std::left << std::setw(15) << proc.processName
                            << std::left << std::setw(25) << proc.type
                            << std::setw(12) << (proc.core == -1 ? "Core: N/A" : "Core: " + std::to_string(proc.core))
                            << std::setw(12) << (std::to_string(proc.gi) + " / " + std::to_string(proc.ci)) << "\n";
                }
            }

            // Write finished processes to the log file
            logFile << "\nFinished Processes:\n";
            for (const auto &proc : realProcesses)
            {
                if (proc.gi >= proc.ci)
                {
                    logFile << std::left << std::setw(15) << proc.processName
                            << std::left << std::setw(25) << proc.type
                            << std::setw(12) << "Finished"
                            << std::setw(12) << (std::to_string(proc.gi) + " / " + std::to_string(proc.ci)) << "\n";
                }
            }

            logFile << "----------------------------------\n\n";
        }

        // Close the log file
        logFile.close();
        char absPath[MAX_PATH];
        if (GetFullPathNameA("./csopesy-log.txt", MAX_PATH, absPath, nullptr) != 0)
        {
            std::cout << "Report Generated at: " << absPath << std::endl;
        }
        else
        {
            std::cerr << "Error finding absolute path." << std::endl;
        }
    }

    // CSOPESY-SMI [NVIDIA-SMI]
    static void opesyosSMI()
    {
        std::unordered_map<std::string, Process> processes = processManager.getAllProcesses();

        if (processes.empty())
        {
            std::cout << "No processes are currently running." << std::endl;
        }
        else
        {
            cout << "CPU utilization: " << (static_cast<double>(FCFSScheduler::getRunningWorkersCount()) / FCFSScheduler::getCPUThreads()) * 100 << "%" << endl;
            cout << "Cores used: " << FCFSScheduler::getRunningWorkersCount() << endl;
            cout << "Cores available: " << FCFSScheduler::getCPUThreads() - FCFSScheduler::getRunningWorkersCount() << "\n" << endl;

            std::vector<DummyProcess> realProcesses;
            int gpuId = 0;

            // Iterate over the real processes and populate their information
            for (const auto& pair : processes)
            {
                const Process& proc = pair.second;
                DummyProcess dp;
                dp.gpu = gpuId++;
                dp.gi = proc.currentLine;
                dp.ci = proc.totalLines;
                dp.pid = proc.pid;
                dp.type = proc.getTimestamp();
                dp.processName = proc.name;
                dp.gpuMemory = "N/A";
                dp.core = proc.cpu;

                realProcesses.push_back(dp);
            }
            std::sort(realProcesses.begin(), realProcesses.end(), compareProcesses);
            Interfaces::displayProcessInfo(realProcesses.size(), realProcesses.data());
        }

        // // Exit message
        // std::cin.get();
        // system("cls");
        // Interfaces::displayHeader();
        // Interfaces::displayMenu();
    }



    // Marquee Console Simulator
    static void marqueeConsole(const std::string& args, ProgramState& state)
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

    // Get Specific Process SMI
    static void processSMI(const std::string& args, ProgramState& state)
    {
        if (state.getCurrentProcess() != ""){
            processManager.displayProcessSMI(state.getCurrentProcess());
        }
        else {
            std::cout << "Command not recognized. Please try again." << endl;
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
            {"process-smi", Commands::processSMI},
            {"debug-scheduler", Commands::debugSchedulerTest},
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
string Commands::currentUserInputCommand = "";
bool Commands::isExit = false;
bool Commands::isSchedulerRunning;

// Initialize command history array and index
std::array<std::string, 3> Commands::commandHistory = { "", "", "" };
int Commands::historyIndex = 0;
Commands::ConfigSettings Commands::configSettings;
std::thread Commands::schedulerThread;
#endif
