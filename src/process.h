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
#include <cstdlib>
#include <windows.h>

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
    std::vector<std::int16_t> pages;
    int timeStartedInMemory;


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
    static int MAX_OVERALL_MEM; // Total memory size in bytes
    static int MEMORY_PER_FRAME;
    static int mode;
    static std::vector<Process *> memory;     // Memory representation with Process pointers
    static std::vector<Process *> runningProcesses; // Running processes in memory
    int processCount;                  // Number of processes in memory
    int totalFragmentation;            // Total external fragmentation in KB
    int logCounter;                    // Counter for log files
    std::mutex memMutex;
    std::condition_variable memCondVar;
    bool isMemoryFreed = true; // Flag to check if memory has been freed

    // Static counters for paged in and paged out pages
    static int pagedInCount;  // Total number of pages paged in
    static int pagedOutCount; // Total number of pages paged out

    // Helper function to convert std::string to std::wstring
    std::wstring stringToWstring(const std::string &str)
    {
        return std::wstring(str.begin(), str.end());
    }

    bool createDirectory(const std::string &path)
    {
        std::wstring widePath = stringToWstring(path); // Convert to wide string
        return CreateDirectory(widePath.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
    }

    std::string generateRandomMemoryData(int length)
    {
        const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::string randomData;
        randomData.reserve(length);

        for (int i = 0; i < length; ++i)
        {
            randomData += charset[rand() % charset.size()];
        }

        return randomData;
    }

    // Function to simulate writing a process's memory to a swap file
    void writeToSwapFile(Process *process)
    {
        if (!process)
        {
            std::cerr << "Error: Null Process pointer passed to writeToSwapFile." << std::endl;
            return;
        }

        // Create the swap directory if it doesn't exist
        if (!createDirectory("./swap"))
        {
            std::cerr << "Error: Failed to create or access the swap directory." << std::endl;
            return;
        }

        // Create a swap file for the process
        std::string swapFileName = "./swap/" + std::to_string(process->pid) + ".csopesy";
        std::ofstream swapFile(swapFileName);

        if (swapFile.is_open())
        {
            // Generate random memory data based on process->currentLine
            std::string memoryData = generateRandomMemoryData(process->currentLine);

            // Write the memory data to the swap file
            swapFile << memoryData;
            swapFile.close();
        }
        else
        {
            std::cerr << "Failed to create swap file for process " << process->pid << std::endl;
        }
        // Increment paged out counter
        if (mode == 1)
            pagedOutCount++;
        Sleep(100); // Simulated Delay
    }

public:
    MemoryManager()
        : processCount(0), totalFragmentation(0), logCounter(0) {}

    static int getMemoryUsed()
    {
        
        int memoryUsed = 0;
        if (mode == 0){
            for (size_t i = 0; i < memory.size(); ++i)
            {
                if (memory[i] != nullptr)
                {
                    memoryUsed++;
                }
            }
        } else if (mode == 1){
            for (size_t i = 0; i < memory.size(); ++i)
            {
                if (memory[i] != nullptr)
                {
                    memoryUsed += MEMORY_PER_FRAME;
                }
            }
        }

        return memoryUsed;
    }

    static int getOverallMemory()
    {
        return MAX_OVERALL_MEM;
    }

    static std::vector<Process *> getUniqueRunningProcesses(){
        std::vector<Process *> uniqueProcesses;
        for (size_t i = 0; i < memory.size(); ++i)
        {
            if (memory[i] != nullptr)
            {
                Process *process = memory[i];
                if (std::find(uniqueProcesses.begin(), uniqueProcesses.end(), process) == uniqueProcesses.end())
                {
                    uniqueProcesses.push_back(process);
                }
            }
        }
        return uniqueProcesses;
    }

    int totalFreeMemory()
    {
        int totalFree = 0;
        int currentFreeBlock = 0;

        // Iterate through memory and sum the sizes of all free blocks
        for (size_t i = 0; i < memory.size(); ++i)
        {
            if (memory[i] == nullptr) // Free memory slot
            {
                currentFreeBlock++;
            }
            else
            {
                if (currentFreeBlock > 0)
                {
                    totalFree += currentFreeBlock; // Add the current free block to total
                    currentFreeBlock = 0;          // Reset for the next block
                }
            }
        }

        // If the last block of memory is free, add it as well
        if (currentFreeBlock > 0)
        {
            totalFree += currentFreeBlock;
        }

        return totalFree; // Return the total free memory available
    }


    int totalFreeFrames()
    {
        int totalFree = 0;
        int currentFreeBlock = 0;

        // Iterate through memory and sum the sizes of all free blocks
        for (size_t i = 0; i < memory.size(); ++i)
        {
            if (memory[i] == nullptr) // Free memory slot
            {
                currentFreeBlock++;
            }
            else
            {
                if (currentFreeBlock > 0)
                {
                    totalFree += currentFreeBlock; // Add the current free block to total
                    currentFreeBlock = 0;          // Reset for the next block
                }
            }
        }

        // If the last block of memory is free, add it as well
        if (currentFreeBlock > 0)
        {
            totalFree += currentFreeBlock;
        }

        return totalFree; // Return the total free memory available
    }

    std::pair<int, int> addToMemory(Process &process)
    {
        std::unique_lock<std::mutex> lock(memMutex);

        // Check if the process is already in memory
        for (size_t i = 0; i < memory.size(); ++i)
        {
            if (memory[i] != nullptr && memory[i]->pid == process.pid)
            {
                // Process already in memory, return its current memory locations
                return {memory[i]->memLocStart, memory[i]->memLocEnd};
            }
        }

        // Check if the process is in swap space
        std::string swapFileName = "./swap/" + std::to_string(process.pid) + ".csopesy";
        std::ifstream swapFile(swapFileName);

        if (swapFile.good())
        {
            swapFile.close();
            if (std::remove(swapFileName.c_str()) != 0)
            {
                std::cerr << "Error deleting swap file for process " << process.pid << ": " << swapFileName << std::endl;
            }
            Sleep(100); // Simulated Delay
            if (mode == 1)
                pagedInCount++;
            // Return dummy memory locations since process is not in memory but was in swap space
            return {-1, -1};
        }

        int requiredMemory = process.mem;
        int availableMemory = totalFreeMemory();
        int availableFrames = totalFreeMemory();
        int requiredFrames = requiredMemory / MEMORY_PER_FRAME;

        if ((requiredMemory / MEMORY_PER_FRAME) < 1)
        {
            requiredFrames = 1;
        }

        if (mode == 0)
        {

            // If there is not enough free memory, try to remove processes
            if (availableMemory < requiredMemory)
            {
                // Get processes to remove based on available memory
                std::vector<Process *> processesToRemove = getOldestMemoryInProcess(requiredMemory);

                if (processesToRemove.empty())
                {
                    return {-1, -1}; // Not enough memory even after considering removable processes
                }

                // Write processes to swap space before freeing memory
                for (Process *processToRemove : processesToRemove)
                {
                    writeToSwapFile(processToRemove); // Write to swap before removing
                    freeMemory(processToRemove->pid); // Remove from memory
                }

                // Now we are certain that there is enough memory for the new process
            }

            // First-fit allocation
            int startIdx = -1;
            for (int i = 0; i <= memory.size() - requiredMemory; ++i)
            {
                bool fit = true;
                for (int j = 0; j < requiredMemory; ++j)
                {
                    if (memory[i + j] != nullptr) // Check if the slot is already occupied
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
                        memory[i + j] = &process; // Store the process pointer
                    }
                    process.memLocStart = startIdx;
                    process.memLocEnd = startIdx + requiredMemory - 1;
                    process.timeStartedInMemory = std::time(nullptr);
                    processCount++;
                    break;
                }
            }

            if (startIdx == -1)
            {
                return {-1, -1}; // Memory allocation failed
            }

            return {process.memLocStart, process.memLocEnd};
        } else if (mode == 1) {
            // If there is not enough free memory, try to remove processes
            if (availableFrames < requiredFrames)
            {
                // Get processes to remove based on available memory
                std::vector<Process *> processesToRemove = getOldestMemoryInProcess(requiredFrames);

                if (processesToRemove.empty())
                {
                    return {-1, -1}; // Not enough memory even after considering removable processes
                }

                // Write processes to swap space before freeing memory
                for (Process *processToRemove : processesToRemove)
                {
                    writeToSwapFile(processToRemove); // Write to swap before removing
                    freeMemory(processToRemove->pid); // Remove from memory
                }

                // Now we are certain that there is enough memory for the new process
            }

            // Non-contiguous memory allocation (page/frame allocation)
            int allocatedFrames = 0;
            std::vector<int> allocatedPages;
            for (int i = 0; i < memory.size(); ++i)
            {
                if (memory[i] == nullptr) // Check if the frame is free
                {
                    allocatedFrames++;
                    allocatedPages.push_back(i); // Store the allocated frame (page)
                    memory[i] = &process;        // Store the process pointer in the allocated frame

                    if (allocatedFrames == requiredFrames)
                    {
                
                        process.memLocStart = allocatedPages[0];
                        process.memLocEnd = allocatedPages.back();
                        processCount++;
                        process.timeStartedInMemory = std::time(nullptr);
                        break;
                    }
                }
            }

            // If we couldn't allocate enough frames
            if (allocatedFrames < requiredFrames)
            {
                // Free any allocated pages before returning failure
                for (int page : allocatedPages)
                {
                    memory[page] = nullptr;
                }
                return {-1, -1}; // Memory allocation failed
            }
        }
        return {process.memLocStart, process.memLocEnd};
    }

    static void addRunningProcess(Process *process)
    {
        runningProcesses.push_back(process);
    }
    static void removeRunningProcess(Process *process)
    {
        runningProcesses.erase(std::remove(runningProcesses.begin(), runningProcesses.end(), process), runningProcesses.end());
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
                if (memory[i] != nullptr)
                {
                    Process *process = memory[i]; // Access the process pointer
                    int startIdx = i;

                    // Find the end index for this process in memory
                    while (i < memory.size() && memory[i] == process)
                    {
                        i++;
                    }
                    int endIdx = i;

                    logFile << "\n"
                            << (MAX_OVERALL_MEM - startIdx) << "\n";
                    logFile << process->pid << "\n"; // Log process ID or other process details
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
        for (size_t i = 0; i < memory.size(); ++i)
        {
            if (memory[i] != nullptr && memory[i]->pid == pid)
            {
                memory[i] = nullptr; // Free memory by setting pointer to nullptr
            }
        }
        processCount--;
        isMemoryFreed = true;
        memCondVar.notify_all();
    }

    void clearSwapSpace(int pid){
        std::string swapFileName = "./swap/" + std::to_string(pid) + ".csopesy";
        if (std::remove(swapFileName.c_str()) != 0)
        {
            return;
        }
    }
    void visualizeMemory()
    {
        std::lock_guard<std::mutex> lock(memMutex);
        for (size_t i = 0; i < memory.size(); ++i)
        {
            if (memory[i] == nullptr)
            {
                std::cout << "[Free]";
            }
            else
            {
                std::cout << "[" << memory[i]->pid << "]"; // Print PID or other process details
            }
        }
        std::cout << "\n";
    }

    std::vector<Process *> getOldestMemoryInProcess(int requiredMemory)
    {
        std::vector<Process *> processesToRemove;
        int availableMemory = 0;

        // Map to store unique processes currently in memory and their `timeStartedInMemory`
        std::map<int, Process *> uniqueProcesses;

        // Iterate through memory to find unique processes
        for (size_t i = 0; i < memory.size(); ++i)
        {
            if (memory[i] != nullptr)
            {
                Process *process = memory[i];
                // Only insert processes that haven't been added yet
                if (uniqueProcesses.find(process->pid) == uniqueProcesses.end())
                {
                    uniqueProcesses[process->pid] = process;
                }
            }
        }

        // Extract processes into a vector and sort by `timeStartedInMemory`
        std::vector<Process *> sortedProcesses;
        for (std::map<int, Process *>::iterator it = uniqueProcesses.begin(); it != uniqueProcesses.end(); ++it)
        {
            Process *process = it->second;
            sortedProcesses.push_back(process);
        }

        std::sort(sortedProcesses.begin(), sortedProcesses.end(), [](Process *a, Process *b)
                  { return a->timeStartedInMemory < b->timeStartedInMemory; });

        // Iterate through sorted processes to find enough memory to free
        for (Process *process : sortedProcesses)
        {
            // Skip running processes
            if (std::find(runningProcesses.begin(), runningProcesses.end(), process) != runningProcesses.end())
            {
                continue;
            }

            processesToRemove.push_back(process);

            // Calculate memory occupied by this process
            int freeSpace = 0;
            for (size_t i = process->memLocStart; i <= process->memLocEnd; ++i)
            {
                if (memory[i] == process)
                {
                    freeSpace++;
                }
            }

            availableMemory += freeSpace;

            // Stop if enough memory is freed
            if (availableMemory >= requiredMemory)
            {
                break;
            }
        }

        // If we collected enough processes to free the required memory, return the list
        if (availableMemory >= requiredMemory)
        {
            return processesToRemove;
        }

        // Otherwise, return an empty list
        return {};
    }

    static void setMaxOverallMem(int value)
    {
        MAX_OVERALL_MEM = value;
        memory.resize(MAX_OVERALL_MEM, nullptr);
    }
    static void setMemoryPerFrame(int value)
    {
        MEMORY_PER_FRAME = value;
        if (MAX_OVERALL_MEM == MEMORY_PER_FRAME)
        {
            mode = 0; // Flat Memory
        }
        else
        {
            mode = 1; // Paging Mode
            memory.resize(MAX_OVERALL_MEM / MEMORY_PER_FRAME, nullptr); // Resize per frame representation
        }
    }
    // Static getter functions for paged in and paged out values
    static int getPagedInCount()
    {
        return pagedInCount;
    }

    static int getPagedOutCount()
    {
        return pagedOutCount;
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

    // tracking CPU ticks
    static int idleCpuTicks;
    static int activeCpuTicks;
    static int totalCpuTicks;

    static void initializeCores() {
        availableCores.clear();
        for (int i = 1; i <= numWorkers; ++i) {
            availableCores.push_back(i);
        }
    }

    static void workerThreadFunction(int cpuId)
    {
        int cycleCount = 0;
        std::unordered_map<int, bool> processAllocated;
        while (true)
        {
            Process* processPtr = nullptr;
            {
                std::unique_lock<std::mutex> lock(startStopMtx);
                if (!running && processQueue.isEmpty())
                {
                    break;
                }
                if (processQueue.isEmpty())
                {
                    idleCpuTicks++;
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
                memoryManager.addRunningProcess(processPtr);
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
                        ++activeCpuTicks;
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

                // Process Done Executing
                memoryManager.removeRunningProcess(processPtr);

                // Free memory after the process is done executing (optional)
                if (processPtr->currentLine >= processPtr->totalLines)
                {
                    memoryManager.freeMemory(processPtr->pid);
                    memoryManager.clearSwapSpace(processPtr->pid);
                }
            }
            catch (const std::runtime_error& e)
            {
                printf("Error: %s\n", e.what());
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
    // Getter functions for CPU ticks
    static int getIdleCpuTicks()
    {
        return idleCpuTicks;
    }

    static int getActiveCpuTicks()
    {
        return activeCpuTicks;
    }

    static int getTotalCpuTicks()
    {
        return idleCpuTicks + activeCpuTicks;
    }
};

bool FCFSScheduler::useRoundRobin = false;
ProcessQueue FCFSScheduler::processQueue;
std::vector<std::thread> FCFSScheduler::workerThreads;
bool FCFSScheduler::running = false;
std::mutex FCFSScheduler::startStopMtx;
std::vector<int> FCFSScheduler:: availableCores;
MemoryManager FCFSScheduler::memoryManager;
int MemoryManager::MAX_OVERALL_MEM;
int MemoryManager::MEMORY_PER_FRAME;
int MemoryManager::mode;
std::vector<Process *> MemoryManager::memory;
std::vector<Process *> MemoryManager::runningProcesses;

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

// Initialize static variables
int FCFSScheduler::idleCpuTicks = 0;
int FCFSScheduler::activeCpuTicks = 0;
int FCFSScheduler::totalCpuTicks = 0;

// Initialize static members outside the class definition
int MemoryManager::pagedInCount = 0;
int MemoryManager::pagedOutCount = 0;

#endif