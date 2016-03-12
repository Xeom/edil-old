# Variables for configuring makefile

#  Seperated into seperate file not only for modularity, 
#  but so bash scripts etc. can read it,
 
VERSION=0.0.1

CC=gcc
LD=ar
AR=ar
LN=ln

SRC=src
INC=src/include

WARNINGS=all extra no-unused-parameter pedantic missing-prototypes fatal-errors format
LINKS=ncurses

SRC_NAMES=line vec err cursor ui wintree wincont face chunk textcont
INC_NAMES=line vec err cursor ui wintree wincont face chunk textcont


TEST_NAME=test

DEPS_FILE=deps.mk

