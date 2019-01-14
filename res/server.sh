#!/bin/bash
cd "${0%/*}"

php -S 0.0.0.0:5000 -dextension=../bin/martex.so server.php