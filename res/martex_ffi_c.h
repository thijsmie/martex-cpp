#define FFI_SCOPE "MarTeX"
#define FFI_LIB "libmartexffi.so"

#include <stdbool.h>


typedef enum
{
    MARTEX_RUN_NOP,
    MARTEX_RUN_ENV_COMMAND,
    MARTEX_RUN_GLOBAL_COMMAND,
    MARTEX_RUN_START_ENVIRONMENT,
    MARTEX_RUN_END_ENVIRONMENT,
    MARTEX_RUN_FINISHED
} MartexStatus;

/// Managing structs
typedef struct MartexCtx MartexCtx;
typedef struct MartexModule MartexModule;
typedef struct MartexEnvironment MartexEnvironment;
typedef struct MartexValue MartexValue;

/// Constructors
MartexCtx* makeContext();
MartexModule* makeModule(MartexCtx* ctx, const char* name);
MartexEnvironment* makeEnvironment(MartexModule* module, const char* name);

/// Adding commands
void addGlobalToModule(MartexModule* module, const char* name);
void addCommandToEnvironment(MartexEnvironment* environment, const char* name);

/// Page setting for standard library
void setAllowPage(MartexCtx* ctx, bool allowed);

/// Run and progress parse
void startParse(MartexCtx* ctx, const char* text);
MartexStatus progressParse(MartexCtx* ctx);

/// Results
const char* getResult(MartexCtx* ctx);
bool hasError(MartexCtx* ctx);
const char* getError(MartexCtx* ctx);

/// Data from a paused compiler (which ones are set depends on the status code from progressParse)
const char* getEnvName(MartexCtx* ctx);
int getEnvModId(MartexCtx* ctx);
const char* getCommandName(MartexCtx* ctx);
int getNumArguments(MartexCtx* ctx);
MartexValue* getArgument(MartexCtx* ctx, int arg);
const char* valueAsString(MartexCtx* ctx, MartexValue*);

/// Resume parsing by returning
void returnVoid(MartexCtx* ctx);
void returnValue(MartexCtx* ctx, MartexValue*);
void returnError(MartexCtx* ctx, const char* msg);

/// Value constructors
MartexValue* nullValue(MartexCtx* ctx);
MartexValue* stringValue(MartexCtx* ctx, const char* content);
MartexValue* multiValue(MartexCtx* ctx, MartexValue*, MartexValue*);
MartexValue* htmlValue(MartexCtx* ctx, const char* tag, MartexValue*);
MartexValue* infoValue(MartexCtx* ctx, const char* tag, MartexValue*);
MartexValue* attrValue(MartexCtx* ctx, const char* tag, const char* value);

/// Clear up a context (will take care of all allocated Modules, Environments, Strings and Values)
void freeContext(MartexCtx*);
