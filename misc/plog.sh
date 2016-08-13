#!/bin/sh

dir=$(dirname $0)

case $1 in
info)
    $dir/bracket.sh 35 i 1>&2
    ;;
warn)
    $dir/bracket.sh 33 w 1>&2
    ;;
err)
    $dir/bracket.sh 31 ! 1>&2
    ;;
succ)
    $dir/bracket.sh 32 . 1>&2
    ;;
*)
    exit
    ;;
esac

content=$(echo $* | cut -d" " -f2-)
printf " %s\n" "$content" 1>&2

