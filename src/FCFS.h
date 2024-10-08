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
#include "process.h" 
#include "systemos.h" 

class ProcessQueue {
private:
    std::queue<Process> processes;
    std::mutex mtx;
    std::condition_variable cv;

public:
    void addProcess(const Process& process) {
        std::unique_lock<std::mutex> lock(mtx);
        processes.push(process);
        cv.notify_one();
    }

    Process getProcess() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !processes.empty(); });
        Process process = processes.front();
        processes.pop();
        return process;
    }

    bool isEmpty() {
        std::lock_guard<std::mutex> lock(mtx);
        return processes.empty();
    }
};

class FCFSScheduler {
private:
    static ProcessQueue processQueue; 
    static std::vector<std::thread> workerThreads; 
    static bool running; 
    static const int numWorkers = 2; 
    static SystemOS* system;

    static void workerThreadFunction(int cpuId) {
        while (running) {
            if (!processQueue.isEmpty()) {
                Process process = processQueue.getProcess();
                // Simulating process execution
                process.printLogs(cpuId); // Call printLogs from Process structure
                std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate execution time
            }
        }
    }

    static void schedulerThreadFunction() {
        // Example: Simulate adding processes to the queue
        for (int i = 0; i < 10; ++i) {
            Process newProcess = { "Process" + std::to_string(i + 1), 1, 100, std::time(nullptr), i + 1 };
            processQueue.addProcess(newProcess);
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate time between process arrivals
        }
        // Signal that the scheduler has finished adding processes
        running = false;
    }

public:
    static void start(SystemOS* sys) { 
        system = sys; // Initialize with a SystemOS pointer
        running = true; // Set the running flag to true

        // Create worker threads
        for (int i = 0; i < numWorkers; ++i) {
            workerThreads.emplace_back(workerThreadFunction, i + 1);
        }

        std::thread schedulerThread(schedulerThreadFunction);
        schedulerThread.join();
        
        // Join worker threads after scheduler is done
        for (auto& worker : workerThreads) {
            worker.join();
        }
    }

    ~FCFSScheduler() {
        running = false; // Signal workers to stop
    }
};

ProcessQueue FCFSScheduler::processQueue;
std::vector<std::thread> FCFSScheduler::workerThreads;
bool FCFSScheduler::running = false;
SystemOS* FCFSScheduler::system = nullptr;

#endif 
