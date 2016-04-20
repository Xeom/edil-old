
#!/bin/bash

echo > tmp

make -C .. lib &&
    if [[ $* == *debug* ]]; then
        if [[ ! -f stfu-python.supp ]]; then
           curl https://svn.python.org/projects/python/trunk/Misc/valgrind-python.supp > stfu-python.supp
        fi
        if [[ $* == *full* ]]; then
            valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all --suppressions=stfu-python.supp --suppressions=stfu.supp python3 -B __init__.py 2>tmp
        else
            valgrind --track-origins=yes --leak-check=full --show-leak-kinds=possible,indirect,definite --suppressions=stfu-python.supp --suppressions=stfu.supp python3 -B __init__.py 2>tmp
        fi
    else
        python3 -B __init__.py 2>tmp
    fi

cat tmp
