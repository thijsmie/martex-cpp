#!/bin/bash
cd "${0%/*}"

php -dextension=../bin/martex.so generate.php
