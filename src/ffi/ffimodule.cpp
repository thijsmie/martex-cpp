#include "ffimodule.hpp"
#include "ffimartex.hpp"
#include "ffienvironment.hpp"


FFIModule::FFIModule(std::string name, MartexCtx* mycontext) : name(name), mycontext(mycontext), myEnvironments(), myGlobals(), env_uniq(0)
{
}

void FFIModule::AddGlobal(std::string gname) { myGlobals.push_back(gname); }
void FFIModule::AddEnvironment(std::string ename, std::vector<std::string> commands) { myEnvironments.insert({ename, commands}); }
std::vector<std::string> FFIModule::GetGlobals() { return myGlobals; }
std::vector<std::string> FFIModule::GetEnvs() { 
    std::vector<std::string> ret; 
    for (auto const& imap : myEnvironments) 
        ret.push_back(imap.first);
    return ret; 
}

std::shared_ptr<Environment> FFIModule::MakeEnv(std::string ename, std::shared_ptr<Environment> parent)
{
    return std::make_shared<FFIEnvironment>(mycontext, ename, env_uniq++, myEnvironments[ename], parent);
}

Value FFIModule::RunGlobal(std::shared_ptr<Environment> env, Token gname, std::vector<Value> args)
{
    std::unique_lock<std::mutex> lk(mycontext->cv_m);

    mycontext->state = MartexParseState::RunAction;
    mycontext->action_error = false;
    mycontext->action = ActionType::RunGlobalCommand;

    mycontext->action_envname = name;
    mycontext->action_name = gname.GetLexeme();
    mycontext->action_args = std::move(args);

    mycontext->cv.notify_one();
    mycontext->cv.wait(lk, [this]{ return mycontext->state == MartexParseState::RunReturn; });
    mycontext->state = MartexParseState::NoAction;

    if (mycontext->action_error) {
        throw RuntimeError(gname, mycontext->action_error_message);
    }

    Value return_value;
    std::swap(return_value, mycontext->action_return);
    
    return return_value;
}
