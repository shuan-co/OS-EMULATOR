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
#include <random>
#include <unordered_map>

struct Process
{
    std::string name;
    int currentLine;
    int totalLines;
    std::time_t creationTime;
    int pid;
    int cpu;

    std::string getTimestamp() const
    {
        std::ostringstream oss;
        std::tm *timeInfo = std::localtime(&creationTime);
        oss << std::put_time(timeInfo, "(%m/%d/%Y %I:%M:%S %p)");
        return oss.str();
    }

    void printLogs(int core) const
    {
        // Logic For Printing Logs
        Sleep(0); // Simulated Delay
        // std::ofstream logFile;
        // std::string fileName = "./logs/" + name + ".txt";
        // logFile.open(fileName, std::ios::app);

        // if (!logFile.is_open())
        // {
        //     std::cerr << "Error: Unable to open log file: " << fileName << std::endl;
        //     return;
        // }

        // logFile.seekp(0, std::ios::end);
        // if (logFile.tellp() == 0)
        // {
        //     logFile << "Process name: " << name << "\n";
        //     logFile << "Logs:" << "\n\n";
        // }

        // logFile << getTimestamp() << " Core:" << core << " \"Hello world from " << name << "\"\n";

        // logFile.close();
    }
};

class ProcessQueue
{
private:
    std::queue<Process *> processes;
    std::mutex mtx;
    std::condition_variable cv;
    


public:
    static int quantumSplice;

    static void setQuantumSplice(int quantum)
    {
        quantumSplice = quantum;
    }

    void addProcess(Process *process)
    {
        std::unique_lock<std::mutex> lock(mtx);
        processes.push(process);
        cv.notify_one();
    }

    Process *getProcessFCFS()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]
                { return !processes.empty(); });
        Process *process = processes.front();
        processes.pop();
        return process;
    }

    Process* getProcessRR()
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (processes.empty()) return nullptr;
        Process* process = processes.front();
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
    static int numWorkers;
    static std::mutex startStopMtx;
    static bool useRoundRobin;
    static int currentCpuId;
    static int runningWorkersCount;
    static std::vector<int> availableCores;
    static int delayPerExec;

    static void initializeCores() {
        availableCores.clear();
        for (int i = 1; i <= numWorkers; ++i) {
            availableCores.push_back(i);
        }
    }

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
                if (useRoundRobin)
                {
                    processPtr = processQueue.getProcessRR();
                }
                else
                {
                    processPtr = processQueue.getProcessFCFS();
                }

                // Assign CPU cores in round-robin order
                if (!availableCores.empty()) {
                    processPtr->cpu = availableCores.front();
                    availableCores.erase(availableCores.begin());
                }
                

                // Increment runningWorkersCount as this thread is now running a process
                ++runningWorkersCount;
            }

            int timeSpent = 0;
            while (processPtr->currentLine < processPtr->totalLines &&
                   (!useRoundRobin || timeSpent < processQueue.quantumSplice))
            {
                Sleep(delayPerExec);
                processPtr->printLogs(processPtr->cpu);
                {
                    std::unique_lock<std::mutex> lock(startStopMtx);
                    processPtr->currentLine++;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
                timeSpent++;
            }
            // processPtr->cpu = -1;
            // Decrement runningWorkersCount as the process has finished or yielded
            {
                std::unique_lock<std::mutex> lock(startStopMtx);
                availableCores.push_back(processPtr->cpu);
                std::sort(availableCores.begin(), availableCores.end());
                processPtr->cpu = -1;
                --runningWorkersCount;
            }

            if (useRoundRobin && processPtr->currentLine < processPtr->totalLines)
            {
                processQueue.addProcess(processPtr);
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

    static void setDelayPerExec(int value)
    {
        delayPerExec = value;
    }

    static void setRoundRobin(bool click)
    {
        useRoundRobin = click;
    }

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

    static void setCPUThreads(int numCPU)
    {
        numWorkers = numCPU;
        initializeCores();
    }

    static int getCPUThreads()
    {
        return numWorkers;
    }

    static int getRunningWorkersCount()
    {
        std::unique_lock<std::mutex> lock(startStopMtx);
        return runningWorkersCount;
    }
};

bool FCFSScheduler::useRoundRobin = false;
ProcessQueue FCFSScheduler::processQueue;
std::vector<std::thread> FCFSScheduler::workerThreads;
bool FCFSScheduler::running = false;
std::mutex FCFSScheduler::startStopMtx;
std::vector<int> FCFSScheduler:: availableCores;

class ProcessManager
{
private:
    std::unordered_map<std::string, Process> processes;

public:
    bool createProcess(const std::string &name, const int min, const int max)
    {
        if (processes.find(name) == processes.end())
        {
            std::random_device rd;                           
            std::mt19937 gen(rd());                          
            std::uniform_int_distribution<> distr(min, max); 

            int max_instruction_lines = distr(gen); 
            int newPid = processes.size();
            Process newProcess{name, 0, max_instruction_lines, std::time(nullptr), newPid};
            newProcess.cpu = -1;
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


    void displayProcess(const std::string &name)
    {
        std::cout << "================================================================================\n";
        std::cout << " ______   ______     ______     ______     ______     ______     ______    \n"
                     "/\\  == \\ /\\  == \\   /\\  __ \\   /\\  ___\\   /\\  ___\\   /\\  ___\\   /\\  ___\\   \n"
                     "\\ \\  _-/ \\ \\  __<   \\ \\ \\/\\ \\  \\ \\ \\____  \\ \\  __\\   \\ \\___  \\  \\ \\___  \\  \n"
                     " \\ \\_\\    \\ \\_\\ \\_\\  \\ \\_____\\  \\ \\_____\\  \\ \\_____\\  \\/\\_____\\  \\/\\_____\\ \n"
                     "  \\/_/     \\/_/ /_/   \\/_____/   \\/_____/   \\/_____/   \\/_____/   \\/_____/ \n" << endl;;
        std::cout << "================================================================================\n\n";

        Process *process = getProcess(name);
        if (process)
        {
            std::cout << "Created at: " << process->getTimestamp() << std::endl;
            std::cout << "Process: " << process->name << std::endl;
            std::cout << "ID: " << process->pid << std::endl;
            std::cout << "\n";

            std::cout << "Current instruction line: " << process->currentLine << " / " << process->totalLines << std::endl;
            std::cout << "Lines of code: " << process->totalLines << std::endl;
            std::cout << "\n";
        }
        else
        {
            std::cout << "Process " << name << " not found." << std::endl;
            std::cout << "Use command [exit] then press [enter] to return to root terminal" << std::endl;
        }
    }

    void displayProcessSMI(const std::string &name){
        Process *process = getProcess(name);
        if (process)
        {
            std::cout << "\nCreated at: " << process->getTimestamp() << std::endl;
            std::cout << "Process: " << process->name << std::endl;
            std::cout << "ID: " << process->pid << std::endl;
            std::cout << "\n";

            if (process->currentLine < process->totalLines)
            {
                std::cout << "Current instruction line: " << process->currentLine << " / " << process->totalLines << std::endl;
                std::cout << "Lines of code: " << process->totalLines << std::endl;
                std::cout << "\n";
            }
            else
            {
                std::cout << "Finished!\n\n";
            }
        }
        else
        {
            std::cout << "Process " << name << " not found." << std::endl;
            std::cout << "Use command [exit] then press [enter] to return to root terminal" << std::endl;
        }
    }
};

int FCFSScheduler::numWorkers = 1;
int FCFSScheduler::currentCpuId = 1;
int FCFSScheduler::runningWorkersCount = 0;
int ProcessQueue::quantumSplice = 4;
int FCFSScheduler::delayPerExec = 0;
#endif