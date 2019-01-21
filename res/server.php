<?php

if ($_SERVER['REQUEST_METHOD'] === 'POST')
{
    $a = new \MarTeX\MarTeX();
    $a->parse(file_get_contents('php://input'));
   
    if($a->HasError())
    {
        echo "<span style='color:red;'>";
        echo $a->GetError();
        echo "</span><br><br>";
    }
    echo $a->GetResult();

    return;
}

if ( $_SERVER['REQUEST_URI'] !== "/")
    return false;
?>
<html>
    <head>
        <link href="martex.css" rel="stylesheet" type="text/css"/>
    </head>
<body>
<textarea cols="100" rows="20" id="data" onchange="update();" oninput="update();"></textarea>
<div id="response"></div>
</body>
<script type="text/javascript">
var dt = document.getElementById("data");
var rp = document.getElementById("response");

function update()
{
    var t0 = performance.now();
    var xhr = new XMLHttpRequest();

    xhr.onreadystatechange = function () {
    if (this.readyState != 4) return;

    if (this.status == 200) {
        rp.innerHTML = this.responseText;
        var t1 = performance.now();
        console.log("Martex roundtrip was " + (t1 - t0) + " milliseconds.")
        }
    };

    xhr.open("POST", "/", true);
    xhr.send(dt.value);
}
</script>
</html>