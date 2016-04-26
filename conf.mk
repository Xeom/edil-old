
# Variables for configuring makefile

#  Seperated into seperate file not only for modularity, 
#  but so bash scripts etc. can read it,
 
VERSION=0.0.1

CC=gcc
LD=ar
AR=ar
LN=ln

SRC=src
INC=include
OBJ=obj

STD=c99
WARNINGS=all extra pedantic missing-prototypes fatal-errors format conversion declaration-after-statement
LINKS=ncurses
INCLUDES=include/ src/ test/

TEST_NAMES=unit_table unit_vec

SRC_NAMES=\
    core                   \
                           \
    err                    \
    hook                   \
    callback               \
                           \
        win/label          \
        win/pos            \
        win/select         \
        win/size           \
        win/win            \
        win/iter           \
                           \
        buffer/buffer      \
        buffer/chunk       \
        buffer/line        \
                           \
        container/table    \
        container/vec      \
        container/hashes   \
        container/bivec    \
                           \
        ui/ui              \
        ui/sbar            \
        ui/face            \
        ui/util            \
                           \
            ui/win/win     \
            ui/win/content \
            ui/win/frame   \
                           \
        io/file            \
        io/key             \
        io/keymap


DEPS_FILE=deps.mk

