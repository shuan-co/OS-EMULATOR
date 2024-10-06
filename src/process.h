#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>

struct Process
{
    std::string name;
    int currentLine;
    int totalLines;
    std::time_t creationTime;
    int pid;

    std::string getTimestamp() const
    {
        std::ostringstream oss;
        std::tm *timeInfo = std::localtime(&creationTime);
        oss << std::put_time(timeInfo, "(%m/%d/%Y %I:%M:%S %p)");
        return oss.str();
    }

    void printLogs(int core) const
    {
        std::ofstream logFile;
        std::string fileName = "./logs/" + name + ".txt";
        logFile.open(fileName, std::ios::app);

        if (!logFile.is_open())
        {
            std::cerr << "Error: Unable to open log file: " << fileName << std::endl;
            return;
        }

        // Write header if the file is empty
        logFile.seekp(0, std::ios::end);
        if (logFile.tellp() == 0)
        {
            logFile << "Process name: " << name << "\n";
            logFile << "Logs:" << "\n\n";
        }

        // Append new log entry
        logFile << getTimestamp() << " Core:" << core << " \"Hello world from " << name << "\"\n";

        logFile.close();
    }
};

class ProcessManager {
private:
    std::unordered_map<std::string, Process> processes;

public:
    bool createProcess(const std::string& name) {
        if (processes.find(name) == processes.end()) {
            Process newProcess{ name, 1, 100, std::time(nullptr) };
            processes[name] = newProcess;
            return true;
        } else {
            return false;
        }
    }

    Process* getProcess(const std::string& name) {
        auto it = processes.find(name);
        return (it != processes.end()) ? &(it->second) : nullptr;
    }

    int getProcessCount() const {
        return processes.size();
    }

    void displayAllProcesses() const
    {
        for (const auto &pair : processes)
        {
            const Process &process = pair.second;
            std::cout << "\nProcess name: " << process.name << std::endl;
            std::cout << "Current line: " << process.currentLine << " / " << process.totalLines << std::endl;
            std::cout << "Created at: " << process.getTimestamp() << std::endl;
            std::cout << "PID: " << process.pid << "\n";
        }
        std::cout << "Total processes: " << getProcessCount() << "\n";
    }

    void displayProcess(const std::string& name) {

        std::cout << "\
                _____                                          \n\
                |  __ \\                                         \n\
                | |__) | __ ___   ___ ___  ___ ___  ___  ___    \n\
                |  ___/ '__/ _ \\ / __/ _ \\/ __/ __|/ _ \\/ __|   \n\
                | |   | | | (_) | (_|  __/\\__ \\__ \\  __/\\__ \\   \n\
                |_|   |_|  \\___/ \\___\\___||___/___/\\___||___/   \n\
        " << std::endl;

        Process* process = getProcess(name);
        if (process) {
            std::cout << "Process name: " << process->name << std::endl;
            std::cout << "Current line: " << process->currentLine << " / " << process->totalLines << std::endl;
            std::cout << "Created at: " << process->getTimestamp() << std::endl;
        }
        else {
            std::cout << "Process not found." << std::endl;
        }
    }
};

#endif