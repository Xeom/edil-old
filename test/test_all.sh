#!/bin/bash

testfiles=$(echo *.c | cut -d"." -f1)

for tf in $testfiles; do
	make -C .. test/$tf && ./$tf
done
