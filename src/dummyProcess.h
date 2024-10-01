#ifndef DUMMYPROCESS_H
#define DUMMYPROCESS_H

#include <string.h>

// Dummy Process Class
class DummyProcess
{
    public:
        int gpu;
        int gi;
        int ci;
        int pid;
        string type;
        string processName;
        string gpuMemory;
        int usage;
};

#endif