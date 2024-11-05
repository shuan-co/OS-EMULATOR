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
    int mem;
    int memLocStart;
    int memLocEnd;

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
    std::queue<Process *> processes; // Queue to hold processes
    std::mutex mtx;                  // Mutex for thread safety
    std::condition_variable cv;      // Condition variable for synchronization

public:
    static int quantumSplice; // Quantum time slice for scheduling

    static void setQuantumSplice(int quantum)
    {
        quantumSplice = quantum;
    }

    void addProcess(Process *process)
    {
        std::unique_lock<std::mutex> lock(mtx);
        processes.push(process);
        cv.notify_one(); // Notify one waiting thread that a new process has been added
    }

    Process *getProcessFCFS()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]
                { return !processes.empty(); }); // Wait until a process is available
        Process *process = processes.front();    // Get the front process
        processes.pop();                         // Remove it from the queue
        return process;
    }

    Process *getProcessRR()
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (processes.empty())
            return nullptr;                   // Return null if the queue is empty
        Process *process = processes.front(); // Get the front process
        processes.pop();                      // Remove it from the queue
        return process;
    }

    bool isEmpty()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return processes.empty(); // Check if the queue is empty
    }

    std::vector<Process *> getProcessesSnapshot()
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::queue<Process *> tempQueue = processes; // Create a temporary queue
        std::vector<Process *> snapshot;             // Vector to hold the snapshot of processes
        while (!tempQueue.empty())
        {
            snapshot.push_back(tempQueue.front()); // Add each process to the snapshot
            tempQueue.pop();                       // Remove it from the temporary queue
        }
        return snapshot;
    }

    // Move a specific process to the back of the queue
    void moveToBack(Process *process)
    {
        std::unique_lock<std::mutex> lock(mtx);
        std::queue<Process *> tempQueue;

        // Move all processes to a temporary queue, excluding the one we want to move
        while (!processes.empty())
        {
            if (processes.front() != process)
            {
                tempQueue.push(processes.front());
            }
            processes.pop();
        }

        // Push the process to the back of the temporary queue
        tempQueue.push(process);

        // Restore the original queue with the modified order
        processes = tempQueue;
    }
};

class MemoryManager
{
private:
    const int MAX_OVERALL_MEM = 16384; // Total memory size in bytes
    std::vector<int> memory;           // Memory representation
    int processCount;                  // Number of processes in memory
    int totalFragmentation;            // Total external fragmentation in KB
    int logCounter;                    // Counter for log files
    std::mutex memMutex;
    std::condition_variable memCondVar;
    bool isMemoryFreed = true; // Flag to check if memory has been freed

public:
    MemoryManager()
        : memory(MAX_OVERALL_MEM, -1), processCount(0), totalFragmentation(0), logCounter(0) {}

    std::pair<int, int> addToMemory(Process &process)
    {
        std::unique_lock<std::mutex> lock(memMutex);

        int requiredMemory = process.mem;
        int startIdx = -1;

        // First-fit allocation
        for (int i = 0; i <= memory.size() - requiredMemory; ++i)
        {
            bool fit = true;
            for (int j = 0; j < requiredMemory; ++j)
            {
                if (memory[i + j] != -1)
                {
                    fit = false;
                    break;
                }
            }

            if (fit)
            {
                startIdx = i;
                for (int j = 0; j < requiredMemory; ++j)
                {
                    memory[i + j] = process.pid;
                }
                process.memLocStart = startIdx;
                process.memLocEnd = startIdx + requiredMemory - 1;
                processCount++;
                isMemoryFreed = false; // Memory is now occupied
                break;
            }
        }

        if (startIdx == -1)
        {
            throw std::runtime_error("Not enough memory to allocate for process.");
        }

        return {process.memLocStart, process.memLocEnd};
    }

    void logMemorySnapshot()
    {
        std::lock_guard<std::mutex> lock(memMutex);

        std::string filename = "./logs/memory_stamp_" + std::to_string(logCounter) + ".txt";

        if (std::ifstream(filename.c_str()))
        {
            std::cout << "Log file already exists: " << filename << ", skipping log creation.\n";
            return;
        }

        std::ofstream logFile(filename);
        if (logFile.is_open())
        {
            std::time_t now = std::time(nullptr);
            std::tm *timeInfo = std::localtime(&now);
            logFile << "Timestamp: " << std::put_time(timeInfo, "(%m/%d/%Y %I:%M:%S %p)") << "\n";
            logFile << "Number of processes in memory: " << processCount << "\n";
            logFile << "Total external fragmentation in KB: " << totalFragmentation << "\n";

            logFile << "\n----end---- = " << MAX_OVERALL_MEM << "\n";

            for (size_t i = 0; i < memory.size(); ++i)
            {
                if (memory[i] != -1)
                {
                    int pid = memory[i];
                    int startIdx = i;

                    while (i < memory.size() && memory[i] == pid)
                    {
                        i++;
                    }
                    int endIdx = i;

                    logFile << "\n" << (MAX_OVERALL_MEM - startIdx) << "\n";
                    logFile << pid << "\n";
                    logFile << (MAX_OVERALL_MEM - endIdx) << "\n\n";
                }
            }

            logFile << "\n----start---- = 0\n";

            logFile.close();
        }
        else
        {
            std::cerr << "Error: Unable to open log file: " << filename << std::endl;
        }

        logCounter++;
    }

    int getProcessCount() const
    {
        return processCount;
    }

    int getTotalFragmentation() const
    {
        return totalFragmentation;
    }

    void freeMemory(int pid)
    {
        std::unique_lock<std::mutex> lock(memMutex);

        for (size_t i = 0; i < memory.size(); ++i)
        {
            if (memory[i] == pid)
            {
                memory[i] = -1;
            }
        }
        processCount--;

        isMemoryFreed = true;
        memCondVar.notify_all();
    }

    void visualizeMemory()
    {
        std::lock_guard<std::mutex> lock(memMutex);
        for (size_t i = 0; i < memory.size(); ++i)
        {
            if (memory[i] == -1)
            {
                std::cout << "[Free]";
            }
            else
            {
                std::cout << "[" << memory[i] << "]";
            }
        }
        std::cout << "\n";
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
    static MemoryManager memoryManager;

    static void initializeCores() {
        availableCores.clear();
        for (int i = 1; i <= numWorkers; ++i) {
            availableCores.push_back(i);
        }
    }

    static void workerThreadFunction(int cpuId)
    {
        int cycleCount = 0;
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

                // Retrieve process from the queue based on scheduling policy
                if (useRoundRobin)
                {
                    processPtr = processQueue.getProcessRR();
                }
                else
                {
                    processPtr = processQueue.getProcessFCFS();
                }

                // Assign CPU cores in round-robin order
                if (!availableCores.empty())
                {
                    processPtr->cpu = availableCores.front();
                    availableCores.erase(availableCores.begin());
                }

                // Increment runningWorkersCount as this thread is now running a process
                ++runningWorkersCount;
            }

            // Try to allocate memory for the process
            try
            {
                std::pair<int, int> memoryLocations = memoryManager.addToMemory(*processPtr);
                processPtr->memLocStart = memoryLocations.first; // Set the memory start location
                processPtr->memLocEnd = memoryLocations.second;  // Set the memory end location

                int timeSpent = 0;
                while (processPtr->currentLine < processPtr->totalLines &&
                       (!useRoundRobin || timeSpent < processQueue.quantumSplice))
                {
                    cycleCount++;

                    // Only execute instruction after X delay cycles + 1 execution cycle
                    if (cycleCount % (delayPerExec + 1) == 0) // Execute on the cycle after X delay cycles
                    {
                        processPtr->printLogs(processPtr->cpu);
                        {
                            std::unique_lock<std::mutex> lock(startStopMtx);
                            processPtr->currentLine++;
                        }
                        timeSpent++;
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }

                if (timeSpent == processQueue.quantumSplice)
                {
                    memoryManager.logMemorySnapshot();
                }

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

                // Free memory after the process is done executing (optional)
                memoryManager.freeMemory(processPtr->pid);
            }
            catch (const std::runtime_error &e)
            {
                // If memory allocation fails, move the process back to the queue
                processQueue.moveToBack(processPtr);
                // Decrement runningWorkersCount since we didn't execute the process
                {
                    std::unique_lock<std::mutex> lock(startStopMtx);
                    availableCores.push_back(processPtr->cpu);
                    std::sort(availableCores.begin(), availableCores.end());
                    processPtr->cpu = -1;
                    --runningWorkersCount;
                }
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
MemoryManager FCFSScheduler::memoryManager;

class ProcessManager
{
private:
    std::unordered_map<std::string, Process> processes;

public:
    bool createProcess(const std::string &name, const int min, const int max, const int mem)
    {
        if (processes.find(name) == processes.end())
        {
            std::random_device rd;                           
            std::mt19937 gen(rd());                          
            std::uniform_int_distribution<> distr(min, max); 

            int max_instruction_lines = distr(gen); 
            int newPid = processes.size();
            Process newProcess{name, 0, max_instruction_lines, std::time(nullptr), newPid, -1, mem};
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