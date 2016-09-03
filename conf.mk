
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
FOPTIONS=no-diagnostics-show-caret \
         diagnostics-color=always

WARNINGS=all no-unused-parameter extra pedantic \
         missing-prototypes fatal-errors format \
         conversion declaration-after-statement \
         error

LINKS=ncurses

INCLUDES=include/ src/ test/

TEST_NAMES=unit_table unit_vec unit_buffer unit_ui_text

SRC_NAMES=\
    core                   \
                           \
    err                    \
    hook                   \
    callback               \
    mode                   \
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
        buffer/deferline   \
        buffer/log         \
                           \
        container/table    \
        container/vec      \
        container/hashes   \
                           \
        ui/ui              \
        ui/sbar            \
        ui/face            \
        ui/util            \
        ui/updates         \
        ui/text            \
                           \
            ui/win/win     \
            ui/win/content \
            ui/win/frame   \
                           \
        io/file            \
        io/key             \
        io/keymap          \
        io/listener        \
                           \
        cursor/cursor      \
        cursor/point       \
        cursor/region      \
        cursor/snap


DEPS_FILE=deps.mk

