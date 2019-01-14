<?php

$success = true;


function runstest($name, $mods, $tx)
{
    echo "Running $name, ";
    $start = microtime(true);
    $a = new \MarTeX\MarTeX();
    foreach($mods as $mod)
        $a->RegisterModule($mod);
    $a->parse($tx);
    $finish = microtime(true);

    if (! $a->HasError()) {
        echo "success, took " . round(($finish - $start) * 1000.0) . "ms\n";
    }
    else {
        global $success;
        $success = false;
        echo "fail, ";
        echo "errorinfo:\n";
        echo $a->GetError();
    }
}

function runtest($name, $tx, $exp, $error_expected=false)
{
    echo "Running $name, ";
    $start = microtime(true);
    $a = new \MarTeX\MarTeX();
    $a->parse($tx);
    $finish = microtime(true);

    if (strcmp($a->GetResult(), $exp) == 0 && $a->HasError() == $error_expected) {
        echo "success, took " . round(($finish - $start) * 1000.0 * 1000.0) . "us\n";
    }
    else {
        global $success;
        $success = false;
        echo "fail\n\tInput:\t$tx\n\tExpected:\t$exp\n\tGot:\t" . $a->GetResult() . "\n";
        echo "errorinfo:\n";
        echo $a->GetError();
        echo "\nvardump:\n";
        var_dump($exp);
        var_dump($a->GetResult());
    }
}

function runmtest($name, $mods, $tx, $exp, $error_expected=false)
{
    echo "Running $name, ";
    $start = microtime(true);
    $a = new \MarTeX\MarTeX();
    foreach($mods as $mod)
        $a->RegisterModule($mod);
    $a->parse($tx);
    $finish = microtime(true);

    if (strcmp($a->GetResult(), $exp) == 0 && $a->HasError() == $error_expected) {
        echo "success, took " . round(($finish - $start) * 1000.0 * 1000.0) . "us\n";
    }
    else {
        global $success;
        $success = false;
        echo "fail\n\tInput:\t$tx\n\tExpected:\t$exp\n\tGot:\t" . $a->GetResult() . "\n";
        echo "errorinfo:\n";
        echo $a->GetError();
        echo "\nvardump:\n";
        var_dump($exp);
        var_dump($a->GetResult());
    }
}

function start($name)
{
    echo "Starting testsuite '$name'.\n";
}

function finish()
{
    global $success;

    if ($success)
    {
        echo "All Tests Passed.\n";
    }
    else
    {
        echo "Failures occurred.\n";
    }

    echo "Peak mem usage: " . (memory_get_peak_usage(false) / 1024) . "kb.\n\n";
}