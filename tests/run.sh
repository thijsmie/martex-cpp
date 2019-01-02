#!/bin/bash
cd "${0%/*}"

for filename in ./*.php; do
	php -dextension=../bin/martex.so $filename
done
