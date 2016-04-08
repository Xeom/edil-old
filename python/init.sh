#!/bin/bash

make -C .. lib &&
    if [[ $* == *debug* ]]; then
        if [[ ! -f stfu-python.supp ]]; then
           curl https://svn.python.org/projects/python/trunk/Misc/valgrind-python.supp > stfu-python.supp
        fi
        valgrind --suppressions=stfu-python.supp python3 __init__.py 2>tmp
        cat tmp
    else
        python3 __init__.py
    fi
