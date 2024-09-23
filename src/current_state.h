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

public:
    ProgramState() : currentContext(Context::MAIN_MENU), currentProcessName("") {}

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
};

#endif