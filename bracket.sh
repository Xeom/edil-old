#!/bin/sh

col_dim() {
    printf "\033[0;%dm" $1
}

col_bri() {
    printf "\033[1;%dm" $1
}

col_end() {
    printf "\033[0m"
}

printf $(col_bri $1)\[$(col_dim $1)$2$(col_bri $1)\]$(col_end)
