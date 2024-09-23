#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <sstream>

struct Process {
    std::string name;
    int currentLine;
    int totalLines;
    std::time_t creationTime;

    std::string getTimestamp() const {
        std::ostringstream oss;
        std::tm* timeInfo = std::localtime(&creationTime);
        oss << std::put_time(timeInfo, "(%m/%d/%Y %I:%M:%S %p)");
        return oss.str();
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

    void displayProcess(const std::string& name) {

        std::cout << R"(
             _____                                         
            |  __ \                                        
            | |__) | __ ___   ___ ___  ___ ___  ___  ___   
            |  ___/ '__/ _ \ / __/ _ \/ __/ __|/ _ \/ __|  
            | |   | | | (_) | (_|  __/\__ \__ \  __/\__ \  
            |_|   |_|  \___/ \___\___||___/___/\___||___/  
        )" << std::endl;


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