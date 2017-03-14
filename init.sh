#!/bin/bash

# Set some defaults

debuglvl=0
logging=1
makeflags="--warn-undefined-variables -s lib"

# Process our options

while getopts gsb opt; do
    case $opt in
        g) # -g activates debugging. -gg etc. increase the debug level ...
             ((debuglvl++))
            ;;
        s) # -s deactivates logging ...
            logging=0
            ;;
        b) # -b rebuilds edil ...
            makeflags+=" -B"
            ;;
        \?) # Invalid option ...
            misc/plog.sh err "Unrecognised option $opt ..."
            exit
            ;;
    esac
done

# Try and build edil...

if ! make $makeflags; then
    misc/plog.sh err "Failed to build lib ..."
    exit
fi

# Set up valgrind flags for the debug level

case $debuglvl in
    0)
        vgrind=0
        ;;
    1)
        vgrind=1
        vflags="--track-origins=yes --leak-check=full --show-leak-kinds=definite\
                --suppressions=stfu-python.supp --suppressions=stfu.supp"
        ;;
    2)
        vgrind=1
        vflags="--track-origins=yes --leak-check=full --show-leak-kinds=all\
                --suppressions=stfu-python.supp --suppressions=stfu.supp"
        ;;
esac

# Build our command

cmd="python3 -B python/__init__.py"

if [[ $vgrind  == 1 ]]; then
    cmd="valgrind $vflags $cmd"
fi

echo>tmp

# Run the command, with logging if necessary

if [[ $logging == 1 ]]; then
    $cmd 2>tmp
else
    $cmd 2>/dev/null
fi

# Print out the logging ..

cat tmp
