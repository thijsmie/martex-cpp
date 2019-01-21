<?php

include __DIR__.'/../modules/itemize.php';
include __DIR__.'/../modules/figure.php';
include __DIR__.'/../modules/document.php';

$input = file_get_contents(__DIR__ . "/documentation.tex");

$start = microtime(true);
$Tex = new \MarTeX\MarTeX();
$Tex->allowPage();
$Tex->registerModule("\\MarTeX\\DocumentModule");
$Tex->Parse($input);
$finish = microtime(true);
echo "Rendering docs took " . round(($finish - $start) * 1000.0) . "ms.";

echo $Tex->GetError()."\n";
file_put_contents(__DIR__ . "/../res/documentation.html",$Tex->getResult());

?>