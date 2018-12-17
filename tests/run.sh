#!/bin/bash
cd "${0%/*}"

for filename in ./*.php; do
	php -d../martex.so $filename
done
