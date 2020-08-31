#include "ffimartex.hpp"
#include "ffienvironment.hpp"


FFIEnvironment::FFIEnvironment(MartexCtx* mycontext, std::string name, int id, std::vector<std::string> commands, std::shared_ptr<Environment> parent) : 
    mycontext(mycontext), Environment(parent), envname(name), identifier(id), commands(commands)
{
}

bool FFIEnvironment::HasCommand(std::string c)
{
    return (std::find(commands.begin(), commands.end(), c) != commands.end());
}

Value FFIEnvironment::RunCommandHere(std::shared_ptr<Environment> env, Token name, std::vector<Value> args)
{
    std::unique_lock<std::mutex> lk(mycontext->cv_m);

    mycontext->state = MartexParseState::RunAction;
    mycontext->action_error = false;
    mycontext->action = ActionType::RunCommand;

    mycontext->action_running_env = this;
    mycontext->action_envname = envname;
    mycontext->action_identifier = identifier;
    mycontext->action_name = name.GetLexeme();
    mycontext->action_args = std::move(args);

    mycontext->cv.notify_one();
    // The other thread won't wake up before we start waiting since we hold the lock till wait is called.
    mycontext->cv.wait(lk, [this]{ return mycontext->state == MartexParseState::RunReturn; });
    mycontext->state = MartexParseState::NoAction;

    if (mycontext->action_error) {
        throw RuntimeError(name, mycontext->action_error_message);
    }

    Value return_value;
    std::swap(return_value, mycontext->action_return);
    
    return return_value;
}

void FFIEnvironment::StartEnvironment(Token begin, Value arg)
{
    std::unique_lock<std::mutex> lk(mycontext->cv_m);

    mycontext->state = MartexParseState::RunAction;
    mycontext->action_error = false;
    mycontext->action = ActionType::StartEnvironment;

    mycontext->action_running_env = this;
    mycontext->action_envname = envname;
    mycontext->action_identifier = identifier;
    mycontext->action_name = begin.GetLexeme();
    
    mycontext->action_args.clear();
    mycontext->action_args.push_back(std::move(arg));

    mycontext->cv.notify_one();
    mycontext->cv.wait(lk, [this]{ return mycontext->state == MartexParseState::RunReturn; });
    mycontext->state = MartexParseState::NoAction;

    if (mycontext->action_error) {
        throw RuntimeError(begin, mycontext->action_error_message);
    }
}

Value FFIEnvironment::EndEnvironment(Token end, Value content)
{
    std::unique_lock<std::mutex> lk(mycontext->cv_m);

    mycontext->state = MartexParseState::RunAction;
    mycontext->action_error = false;
    mycontext->action = ActionType::EndEnvironment;

    mycontext->action_running_env = this;
    mycontext->action_envname = envname;
    mycontext->action_identifier = identifier;
    mycontext->action_name = end.GetLexeme();
    
    mycontext->action_args.clear();
    mycontext->action_args.push_back(std::move(content));

    mycontext->cv.notify_one();
    mycontext->cv.wait(lk, [this]{ return mycontext->state == MartexParseState::RunReturn; });
    mycontext->state = MartexParseState::NoAction;

    if (mycontext->action_error) {
        throw RuntimeError(end, mycontext->action_error_message);
    }

    Value return_value;
    std::swap(return_value, mycontext->action_return);
    
    return return_value;
}
