#ifndef CURRENT_STATE_H
#define CURRENT_STATE_H

#include <string>

enum class Context {
    MAIN_MENU,
    PROCESS_SCREEN
};

class ProgramState {
private:
    Context currentContext;
    std::string currentProcessName;
    bool initialized;

public:
    ProgramState() : currentContext(Context::MAIN_MENU), currentProcessName(""), initialized(false) {}

    void setContext(Context context) {
        currentContext = context;
    }

    Context getContext() const {
        return currentContext;
    }

    void setCurrentProcess(const std::string& processName) {
        currentProcessName = processName;
    }

    std::string getCurrentProcess() const {
        return currentProcessName;
    }

    void setInitialized(bool init) {
        initialized = init;
    }

    bool isInitialized() const {
        return initialized;
    }
};

#endif