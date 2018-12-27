#!/bin/bash
cd "${0%/*}"

for filename in ./*.php; do
	php -dextension=../martex.so $filename
done
