#ifndef FCFS_H
#define FCFS_H

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <unordered_map>

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

        logFile.seekp(0, std::ios::end);
        if (logFile.tellp() == 0)
        {
            logFile << "Process name: " << name << "\n";
            logFile << "Logs:" << "\n\n";
        }

        logFile << getTimestamp() << " Core:" << core << " \"Hello world from " << name << "\"\n";

        logFile.close();
    }
};

class ProcessQueue
{
private:
    std::queue<Process *> processes;
    std::mutex mtx;
    std::condition_variable cv;

public:
    void addProcess(Process *process)
    {
        std::unique_lock<std::mutex> lock(mtx);
        processes.push(process);
        cv.notify_one();
    }

    Process *getProcess()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]
                { return !processes.empty(); });
        Process *process = processes.front();
        processes.pop();
        return process;
    }

    bool isEmpty()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return processes.empty();
    }

        std::vector<Process *> getProcessesSnapshot()
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::queue<Process *> tempQueue = processes;
        std::vector<Process *> snapshot;
        while (!tempQueue.empty())
        {
            snapshot.push_back(tempQueue.front());
            tempQueue.pop();
        }
        return snapshot;
    }
};

class FCFSScheduler
{
private:
    static ProcessQueue processQueue;
    static std::vector<std::thread> workerThreads;
    static bool running;
    static const int numWorkers = 4;
    static std::mutex startStopMtx;

    static void workerThreadFunction(int cpuId)
    {
        while (true)
        {
            Process *processPtr = nullptr;
            {
                std::unique_lock<std::mutex> lock(startStopMtx);
                if (!running && processQueue.isEmpty())
                {
                    break;
                }
                if (processQueue.isEmpty())
                {
                    continue;
                }
                processPtr = processQueue.getProcess();
            }

            while (processPtr->currentLine < processPtr->totalLines)
            {
                processPtr->printLogs(cpuId); 
                {
                    std::unique_lock<std::mutex> lock(startStopMtx);
                    processPtr->currentLine++;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1)); 
            }
        }
    }

    static void ensureSchedulerRunning()
    {
        std::unique_lock<std::mutex> lock(startStopMtx);
        if (!running)
        {
            running = true;
            for (int i = 0; i < numWorkers; ++i)
            {
                workerThreads.emplace_back(workerThreadFunction, i + 1);
            }
        }
    }

    static void stopSchedulerIfIdle()
    {
        std::unique_lock<std::mutex> lock(startStopMtx);
        if (processQueue.isEmpty() && running)
        {
            running = false;
            for (auto &worker : workerThreads)
            {
                if (worker.joinable())
                {
                    worker.join();
                }
            }
            workerThreads.clear();
        }
    }

public:
    static void addProcessToQueue(Process *process)
    {
        processQueue.addProcess(process);
        ensureSchedulerRunning();
    }

    ~FCFSScheduler()
    {
        stopSchedulerIfIdle();
    }

    static std::vector<Process *> getQueuedProcesses()
    {
        return processQueue.getProcessesSnapshot();
    }
};

ProcessQueue FCFSScheduler::processQueue;
std::vector<std::thread> FCFSScheduler::workerThreads;
bool FCFSScheduler::running = false;
std::mutex FCFSScheduler::startStopMtx;

class ProcessManager
{
private:
    std::unordered_map<std::string, Process> processes;

public:
    bool createProcess(const std::string &name)
    {
        if (processes.find(name) == processes.end())
        {
            int newPid = processes.size();
            Process newProcess{name, 0, 100, std::time(nullptr), newPid};
            processes[name] = newProcess;
            FCFSScheduler::addProcessToQueue(&processes[name]); 
            return true;
        }
        else
        {
            return false;
        }
    }

    Process *getProcess(const std::string &name)
    {
        auto it = processes.find(name);
        return (it != processes.end()) ? &(it->second) : nullptr;
    }

    int getProcessCount() const
    {
        return processes.size();
    }

    std::unordered_map<std::string, Process> getAllProcesses() const
    {
       return processes;
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

    void displayProcess(const std::string &name)
    {
    std::cout << "                _____                                          \n"
                 "                |  __ \\                                         \n"
                 "                | |__) | __ ___   ___ ___  ___ ___  ___  ___    \n"
                 "                |  ___/ '__/ _ \\ / __/ _ \\ / __|/ _ \\ / __|   \n"
                 "                | |   | | | (_) | (_|  __/ \\__ \\ \\__ \\  __/ \\__ \\   \n"
                 "                |_|   |_|  \\___/ \\___| \\___||___/___/ \\___||___/   \n"
              << std::endl;


        Process *process = getProcess(name);
        if (process)
        {
            std::cout << "Process name: " << process->name << std::endl;
            std::cout << "Current line: " << process->currentLine << " / " << process->totalLines << std::endl;
            std::cout << "Created at: " << process->getTimestamp() << std::endl;
        }
        else
        {
            std::cout << "Process not found." << std::endl;
        }
    }
};

#endif