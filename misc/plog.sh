#!/bin/sh

case $1 in
info)
    ./bracket.sh 35 i 1>&2
    ;;
warn)
    ./bracket.sh 33 w 1>&2
    ;;
err)
    ./bracket.sh 31 ! 1>&2
    ;;
succ)
    ./bracket.sh 32 . 1>&2
    ;;
*)
    exit
    ;;
esac

content=$(echo $* | cut -d" " -f2-)
printf " %s\n" $content 1>&2

