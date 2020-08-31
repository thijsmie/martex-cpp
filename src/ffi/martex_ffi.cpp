#include "ffimartex.hpp"

#include "core/language/transpiler.hpp"
#include "core/modules/stdlib.hpp"
#include "ffimodule.hpp"

#include <vector>
#include <cstring>
#include <string>
#include <memory>

void transpiler_thread(MartexCtx* ctx, std::string text)
{
    ctx->transpiler->Parse(text);
    {
        std::lock_guard<std::mutex>(ctx->cv_m);
        ctx->state = MartexParseState::RunAction;
        ctx->action = ActionType::ParseFinished;
    }
    ctx->cv.notify_one();
}

extern "C" {
MartexCtx* makeContext()
{
    MartexCtx* ctx = new MartexCtx;

    ctx->state = MartexParseState::NoAction;
    ctx->action_running_env = nullptr;
    ctx->action_envname = "";
    ctx->action_name = "";
    ctx->action_error = false;
    ctx->action_error_message = "";

    ctx->transpiler = nullptr;
    ctx->allow_page = false;
    ctx->result = "";
    ctx->error = "";

    return ctx;
}

void freeContext(MartexCtx* ctx)
{
    for (auto module : ctx->modules)
        delete module;
    for (auto environment : ctx->environments)
        delete environment;
    for (auto value : ctx->values)
        delete value;
    for (auto str : ctx->strings)
        free(str);

    if (ctx->transpiler != nullptr)
        delete ctx->transpiler;

    delete ctx;
}


MartexModule* makeModule(MartexCtx* ctx, const char* name)
{
    MartexModule* module = new MartexModule;
    module->name = std::string(name);
    module->mycontext = ctx;
    ctx->modules.push_back(module);
    return module;
}

void addGlobalToModule(MartexModule* module, const char* name)
{
    module->globals.push_back(std::string(name));
}

MartexEnvironment* makeEnvironment(MartexModule* module, const char* name)
{
    MartexEnvironment* env = new MartexEnvironment;
    env->name = std::string(name);
    module->environments.push_back(env);
    module->mycontext->environments.push_back(env);
    return env;
}

void addCommandToEnvironment(MartexEnvironment* env, const char* name)
{
    env->commands.push_back(std::string(name));
}

void setAllowPage(MartexCtx* ctx, bool allowed)
{
    ctx->allow_page = allowed;
}

void startParse(MartexCtx* ctx, const char* text)
{
    std::vector<std::shared_ptr<Module>> module_inst;
    module_inst.reserve(ctx->modules.size() + 1);
    module_inst.push_back(std::make_shared<StdLib>(ctx->allow_page));

    for (auto module : ctx->modules)
    {
        std::shared_ptr<FFIModule> ffimodule = std::make_shared<FFIModule>(std::string(module->name), ctx);

        for (auto gl : module->globals)
            ffimodule->AddGlobal(gl);
        for (auto env : module->environments)
            ffimodule->AddEnvironment(env->name, env->commands);

        module_inst.push_back(ffimodule);
    }

    // Run the transpiler
    if (ctx->transpiler != nullptr) {
        delete ctx->transpiler;
    }
    ctx->state = MartexParseState::NoAction;
    ctx->transpiler = new Transpiler(module_inst);
    ctx->martex_thread = std::thread(transpiler_thread, ctx, std::string(text));
}

MartexStatus progressParse(MartexCtx* ctx)
{
    std::unique_lock<std::mutex> lk(ctx->cv_m);

    ctx->cv.wait(lk, [ctx]{ return ctx->state == MartexParseState::RunAction; });

    // Why do we not lock here? because we know the other thread is waiting.
    switch(ctx->action)
    {
        case ActionType::StartEnvironment:
            return MartexStatus::MARTEX_RUN_START_ENVIRONMENT;
        case ActionType::EndEnvironment:
            return MartexStatus::MARTEX_RUN_END_ENVIRONMENT;
        case ActionType::RunCommand:
            return MartexStatus::MARTEX_RUN_ENV_COMMAND;
        case ActionType::RunGlobalCommand:
            return MartexStatus::MARTEX_RUN_GLOBAL_COMMAND;
        case ActionType::ParseFinished:
            ctx->martex_thread.join();
            return MartexStatus::MARTEX_RUN_FINISHED;
        case ActionType::NoAction:
            return MartexStatus::MARTEX_RUN_NOP;
    }
    // Unreachable
    return MartexStatus::MARTEX_RUN_NOP;
}

const char* getResult(MartexCtx* ctx)
{
    ctx->result = ctx->transpiler->GetResult();
    return ctx->result.c_str();
}

bool hasError(MartexCtx* ctx)
{
    return ctx->transpiler->HasError();
}

const char* getError(MartexCtx* ctx)
{
    ctx->error = ctx->transpiler->GetErrors();
    return ctx->error.c_str();
}

MartexValue* nullValue(MartexCtx* ctx)
{
    MartexValue* val = new MartexValue;
    val->val = Value(t_null);
    ctx->values.push_back(val);
    return val;
}

MartexValue* stringValue(MartexCtx* ctx, const char* content)
{
    MartexValue* val = new MartexValue;
    val->val = Value(t_string, std::string(content));
    ctx->values.push_back(val);
    return val;
}

MartexValue* multiValue(MartexCtx* ctx, MartexValue* v1, MartexValue* v2)
{
    MartexValue* val = new MartexValue;
    std::vector<Value> values;

    if (v1->val.GetType() == t_multi) {
        Value v = v1->val.explicit_copy();
        values.reserve(values.size() + v.multicontent.size());
        std::move(std::begin(v.multicontent), std::end(v.multicontent), std::back_inserter(values));
    }
    else {
        values.push_back(std::move(v1->val.explicit_copy()));
    }

    if (v1->val.GetType() == t_multi) {
        Value v = v2->val.explicit_copy();
        values.reserve(values.size() + v.multicontent.size());
        std::move(std::begin(v.multicontent), std::end(v.multicontent), std::back_inserter(values));
    }
    else {
        values.push_back(std::move(v2->val.explicit_copy()));
    }

    val->val = Value(std::move(values));
    ctx->values.push_back(val);
    return val;
}


MartexValue* htmlValue(MartexCtx* ctx, const char* tag, MartexValue* content)
{
    MartexValue* val = new MartexValue;

    if (content->val.GetType() == t_multi) {
        Value v = content->val.explicit_copy();
        val->val = Value(std::string(tag), std::move(v.multicontent));
        return val;
    }

    val->val = Value(std::string(tag), content->val.explicit_copy());
    ctx->values.push_back(val);
    return val;
}

MartexValue* infoValue(MartexCtx* ctx, const char* tag, MartexValue* content)
{
    MartexValue* val = new MartexValue;
    std::vector<Value> values;
    values.push_back(std::move(content->val.explicit_copy()));
    val->val = Value(t_info, std::string(tag), std::move(values));
    ctx->values.push_back(val);
    return val;
}

MartexValue* attrValue(MartexCtx* ctx, const char* tag, const char* value)
{
    MartexValue* val = new MartexValue;
    val->val = Value(std::string(tag), std::string(value));
    ctx->values.push_back(val);
    return val;
}

const char* getEnvName(MartexCtx* ctx)
{
    return ctx->action_envname.c_str();
}

int getEnvModId(MartexCtx* ctx)
{
    return ctx->action_identifier;
}

const char* getCommandName(MartexCtx* ctx)
{
    return ctx->action_name.c_str();
}

int getNumArguments(MartexCtx* ctx)
{
    return ctx->action_args.size();
}

MartexValue* getArgument(MartexCtx* ctx, int arg)
{
    MartexValue* val = new MartexValue;
    ctx->values.push_back(val);
    
    val->val = std::move(ctx->action_args[arg]);
    return val;
}

const char* valueAsString(MartexCtx* ctx, MartexValue* val)
{
    std::string ret = val->val.GetContent();
    char* str = (char*) malloc(strlen(ret.c_str()));
    strcpy(str, ret.c_str());
    ctx->strings.push_back(str);
    return str;
}

void returnValue(MartexCtx* ctx, MartexValue* val)
{
    std::swap(ctx->action_return, val->val);
    ctx->action_error = false;

    {
        std::lock_guard<std::mutex> lk(ctx->cv_m);
        ctx->state = MartexParseState::RunReturn;
    }

    ctx->cv.notify_all();
}


void returnError(MartexCtx* ctx, const char* msg)
{
    ctx->action_error_message = std::string(msg);
    ctx->action_error = true;

    {
        std::lock_guard<std::mutex> lk(ctx->cv_m);
        ctx->state = MartexParseState::RunReturn;
    }
    
    ctx->cv.notify_all();
}


void returnVoid(MartexCtx* ctx)
{
    ctx->action_error = false;

    {
        std::lock_guard<std::mutex> lk(ctx->cv_m);
        ctx->state = MartexParseState::RunReturn;
    }

    ctx->cv.notify_all();
}

}