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

WARNINGS=all extra no-unused-parameter pedantic missing-prototypes fatal-errors format conversion
LINKS=ncurses

SRC_NAMES=line container/vec err cursor wintree wincont chunk textcont hook io ui/core ui/sbar ui/win ui/face ui/util callback container/table
DEPS_FILE=deps.mk

