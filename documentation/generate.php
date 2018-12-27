<?php

include __DIR__.'/../modules/itemize.php';
include __DIR__.'/../modules/figure.php';
include __DIR__.'/../modules/document.php';
include __DIR__.'/tools.php';

$input = file_get_contents(__DIR__ . "/documentation.tex");

$start = microtime(true);
$Tex = new \MarTeX\MarTeX();
$Tex->registerModule("\\MarTeX\\ItemizeModule");
$Tex->registerModule("\\MarTeX\\FigureModule");
$Tex->registerModule("\\MarTeX\\DocumentModule");
$Tex->registerModule("\\MarTeX\\DocumentationModule");
$Tex->Parse($input);
$finish = microtime(true);
echo "Rendering docs took " . round(($finish - $start) * 1000.0) . "ms.";

echo $Tex->GetError()."\n";
file_put_contents(__DIR__ . "/documentation.html",$Tex->getResult());

?>