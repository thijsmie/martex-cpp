#pragma once

#include <algorithm>
#include <thread>
#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>

#include "core/language/environment.hpp"
#include "core/language/transpiler.hpp"


enum class ActionType {
    NoAction,
    StartEnvironment,
    EndEnvironment,
    RunCommand,
    RunGlobalCommand,
    ParseFinished
};

typedef enum
{
    MARTEX_RUN_NOP,
    MARTEX_RUN_ENV_COMMAND,
    MARTEX_RUN_GLOBAL_COMMAND,
    MARTEX_RUN_START_ENVIRONMENT,
    MARTEX_RUN_END_ENVIRONMENT,
    MARTEX_RUN_FINISHED
} MartexStatus;

enum class MartexParseState
{
    NoAction,
    RunAction,
    RunReturn
};

struct MartexValue;
struct MartexModule;
struct MartexEnvironment;
struct MartexCtx;

struct MartexValue
{
    Value val;
};

struct MartexModule
{
    std::string name;
    MartexCtx* mycontext;
    std::vector<MartexEnvironment*> environments;
    std::vector<std::string> globals;
};

struct MartexEnvironment
{
    std::string name;
    std::vector<std::string> commands;
};

struct MartexCtx
{
    std::condition_variable cv;
    std::mutex cv_m;
    MartexParseState state;
    
    ActionType action = ActionType::NoAction;
    Environment* action_running_env;
    std::string action_envname;
    std::string action_name;
    int action_identifier;
    std::vector<Value> action_args;
    Value action_return;
    bool action_error;
    std::string action_error_message;

    Transpiler* transpiler;
    bool allow_page;
    std::thread martex_thread;
    std::string result;
    std::string error;

    std::vector<MartexModule*> modules;
    std::vector<MartexEnvironment*> environments;
    std::vector<MartexValue*> values;
    std::vector<char*> strings;
};