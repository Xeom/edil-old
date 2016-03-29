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

WARNINGS=all extra pedantic missing-prototypes fatal-errors format conversion declaration-after-statement
LINKS=ncurses

SRC_NAMES=err wintree wincont hook io callback \
 buffer/chunk buffer/core buffer/line \
 container/table container/vec \
 ui/core ui/sbar ui/win ui/face ui/util \
 io/file
DEPS_FILE=deps.mk

