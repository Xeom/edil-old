pinfo() {
    ../bracket.sh 35 i  1>&2
    printf " %s\n" "$1" 1>&2
}

pwarn() {
    ../bracket.sh 33 w  1>&2
    printf " %s\n" "$1" 1>&2
}

perr() {
    ../bracket.sh 31 !  1>&2
    printf " %s\n" "$1" 1>&2
}

psucc() {
    ../bracket.sh 32 .  1>&2
    printf " %s\n" "$1" 1>&2
}
