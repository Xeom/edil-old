#!/bin/bash

function print_if_unimplemented
{   modulename=$1;functname=$2

    ./is_implemented.py $modulename $functname

	rtn=$?

	if [[ $rtn == 0 ]]; then
        echo $functname is not implemented in $modulename ...
    elif [[ $rtn == 255 ]]; then
		echo error in $modulename
		exit -1
	fi
}

function check_file
{   filename=$1

    nm --defined-only -l lib.so | grep -P $filename.c:[0-9]+$ | grep " T " |
        while read -r functdesc; do

            print_if_unimplemented c.$filename $(echo $functdesc | cut -d" " -f3)
        done
}

for i in c/*.py; do echo $i; done | cut -d"." -f1 | cut -d"/" -f2 |
    while read -r name; do
        check_file $name
    done
