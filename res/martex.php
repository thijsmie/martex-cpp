<?php

namespace MarTeXcpp;



class Module
{
    protected $module;
    protected $ffi;
    protected $context;

    public function globals()
    {
        return array();
    }

    public function environments()
    {
        return array();
    }

    public function __construct($ffi, $context, $registry)
    {
        $this->ffi = $ffi;
        $this->context = $context;
        $this->module = $ffi->makeModule($context, static::class);
        $registry->add_module(static::class, $this);

        foreach($this->globals() as $global)
        {
            $ffi->addGlobalToModule($this->module, $global);
        }

        foreach($this->environments() as $name => $environment)
        {
            $env = $ffi->makeEnvironment($this->module, $name);
            foreach($environment::commands() as $command)
            {
                $ffi->addCommandToEnvironment($env, $command);
            }
            $registry->add_environment_binding($name, $environment);
        }
    }
}

class Environment
{
    public function begin($arg)
    {
        
    }

    public function end($content)
    {
        return $content;
    }

    public static function commands()
    {
        return array();
    }
}

class Registry
{
    private $modules = array();
    private $environments = array();
    private $env_binding = array();

    public function add_module($name, $instance)
    {
        $this->modules[$name] = $instance;
    }

    public function add_environment($name, $instance)
    {
        $this->environments[$name] = $instance;
    }

    public function add_environment_binding($name, $phpobject)
    {
        $this->env_binding[$name] = $phpobject;
    }

    public function get_bound_env($name)
    {
        return $this->env_binding[$name];
    }

    public function module_dispatch($name, $function, $arguments)
    {
        return $this->modules[$name]->$function(...$arguments);
    }

    public function environment_dispatch($name, $function, $arguments)
    {
        return $this->environments[$name]->$function(...$arguments);
    }
}

global $martex_curr_context;

class MarTeX
{
    private static $ffi = null;
    private $allow_page = false;
    private $context = null;
    private $registry = null;
    private $modules = array();

    // Martex parse status codes
    private const MARTEX_RUN_NOP = 0;
    private const MARTEX_RUN_ENV_COMMAND = 1;
    private const MARTEX_RUN_GLOBAL_COMMAND = 2;
    private const MARTEX_RUN_START_ENVIRONMENT = 3;
    private const MARTEX_RUN_END_ENVIRONMENT = 4;
    private const MARTEX_RUN_FINISHED = 5;

    public static function getFFI()
    {
        if (self::$ffi === null)
            self::$ffi = \FFI::load("martex_ffi_c.h"); //\FFI::scope("MarTeX");
        return self::$ffi;
    }

    public function __construct()
    {
        if (self::$ffi === null)
            self::getFFI();
    }

    public function __destruct()
    {
        if ($this->context !== null)
            self::$ffi->freeContext($this->context);
    }

    public function registerModule($module)
    {
        $this->modules[] = $module;
    }

    public function Parse($text)
    {
        global $martex_curr_context;

        /// For safety reasons a context is never reused.
        if ($this->context !== null)
            self::$ffi->freeContext($this->context);

        $this->context = self::$ffi->makeContext();
        $this->registry = new Registry();
        $martex_curr_context = $this->context;

        // Allow page setting
        self::$ffi->setAllowPage($this->context, $this->allow_page);

        // construct a clean set of modules for this parse
        foreach($this->modules as $module)
            new $module(self::$ffi, $this->context, $this->registry);

        // start the parse
        self::$ffi->startParse($this->context, $text);

        // Loop till finished
        $run = true;
        while ($run) {
            $status_code = self::$ffi->progressParse($this->context);
            
            switch($status_code)
            {
                /// Run is finished, result is ready
                case self::MARTEX_RUN_FINISHED:
                    $run = false;
                break;

                /// Run a global module command
                case self::MARTEX_RUN_GLOBAL_COMMAND:
                    $module_name = self::$ffi->getEnvName($this->context);
                    $command_name = self::$ffi->getCommandName($this->context);
                    $arguments = array();
                    for($i = self::$ffi->getNumArguments($this->context) - 1; $i >= 0; $i -= 1)
                    {
                        $arg = self::$ffi->getArgument($this->context, $i);
                        $arguments[$i] = self::$ffi->valueAsString($this->context, $arg);
                    }
                    try {
                        $rvalue = $this->registry->module_dispatch($module_name, $command_name, $arguments);
                        self::$ffi->returnValue($this->context, $rvalue);
                    }
                    catch(\Throwable $e)
                    {
                        self::$ffi->returnError($this->context, $e->getMessage());
                    }
                break;

                /// We shouldn't get this here but if we do just continue
                case self::MARTEX_RUN_NOP:
                break;

                /// Create a new env
                case self::MARTEX_RUN_START_ENVIRONMENT:
                    $environment_name = self::$ffi->getEnvName($this->context);
                    $env_id = self::$ffi->getEnvModId($this->context);
                    $envcls = new $this->registry->get_bound_env($environment_name);
                    $this->registry->add_environment($environment_name . $env_id, new $envcls());
                    try {
                        $this->registry->environment_dispatch($environment_name . $env_id, 'begin', 
                            self::$ffi->valueAsString($this->context, self::$ffi->getArgument($this->context, 0)));
                    }
                    catch(\Throwable $e)
                    {
                        error_log("Could not create php martex env {$environment_name}. This is a configuration error.");
                    }
                    self::$ffi->returnVoid($this->context);
                break;

                /// End an env
                case self::MARTEX_RUN_END_ENVIRONMENT:
                    $environment_name = self::$ffi->getEnvName($this->context);
                    $env_id = self::$ffi->getEnvModId($this->context);
                    try {
                        $rvalue = $this->registry->environment_dispatch($environment_name . $env_id, 'end', 
                            self::$ffi->valueAsString($this->context, self::$ffi->getArgument($this->context, 0)));
                        self::$ffi->returnValue($this->context, $rvalue);
                    }
                    catch(\Throwable $e)
                    {
                        self::$ffi->returnError($this->context, $e->GetMessage());
                    }
                break;

                /// Call a env command
                case self::MARTEX_RUN_ENV_COMMAND:
                    $environment_name = self::$ffi->getEnvName($this->context);
                    $env_id = self::$ffi->getEnvModId($this->context);
                    $command_name = self::$ffi->getCommandName($this->context);
                    $arguments = array();
                    for($i = self::$ffi->getNumArguments($this->context); $i >= 0; $i -= 1)
                    {
                        $arguments[$i] = self::$ffi->valueAsString($this->context, self::$ffi->getArgument($this->context, $i));
                    }
                    try {
                        $rvalue = $this->registry->environment_dispatch($environment_name . $env_id, $command_name, $arguments);
                        self::$ffi->returnValue($this->context, $rvalue);
                    }
                    catch(\Throwable $e)
                    {
                        self::$ffi->returnError($this->context, $e->getMessage());
                    }
                break;
                default:
                    error_log("[FATAL ERROR] MarTeX returned a unknown status code!");
                    return;
                break;
            }
        }
    }

    public function AllowPage()
    {
        $this->allow_page = true;
    }

    public function HasError()
    {
        if ($this->context !== null)    
            return self::$ffi->hasError($this->context);
        return false;
    }

    public function GetErrors()
    {
        if ($this->context !== null)    
            return self::$ffi->getError($this->context);
        return "";
    }

    public function GetResult()
    {
        if ($this->context !== null)    
            return self::$ffi->getResult($this->context);
        return "";
    }
}

function _null()
{
    global $martex_curr_context; 
    $ffi = MarTeX::getFFI();
    
    return $ffi->nullValue($martex_curr_context);
}

function mstr($str)
{
    global $martex_curr_context; 
    $ffi = MarTeX::getFFI();
    
    return $ffi->stringValue($martex_curr_context, $str);
}

function multi(...$content)
{
    global $martex_curr_context; 
    $ffi = MarTeX::getFFI();

    if (count($content) == 0) {
        return _null();
    } else if (count($content) == 1) {
        return $content[0];
    } else {
        $val = $ffi->multiValue($martex_curr_context, $content[0], $content[1]);
        for($i = 2; $i < count($content); $i++)
        {
            $val = $ffi->multiValue($martex_curr_context, $val, $content[$i]);
        }
        return $val;
    }
}

function html($tag, ...$content)
{
    global $martex_curr_context; 
    $ffi = MarTeX::getFFI();

    return $ffi->htmlValue($martex_curr_context, $tag, multi(...$content));
}

function info($tag, ...$content)
{
    global $martex_curr_context; 
    $ffi = MarTeX::getFFI();

    return $ffi->infoValue($martex_curr_context, $tag, multi(...$content));
}

function attr($tag, $value)
{
    global $martex_curr_context; 
    $ffi = MarTeX::getFFI();

    return $ffi->attrValue($martex_curr_context, $tag, $value);
}

