
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

WARNINGS=all extra pedantic missing-prototypes fatal-errors format conversion declaration-after-statement
LINKS=ncurses
INCLUDES=include/ src/

SRC_NAMES=err hook io callback \
  win/label win/pos win/select win/size win/win win/iter \
  buffer/chunk buffer/buffer buffer/line \
  container/table container/vec \
  ui/core ui/sbar ui/face ui/util \
    ui/win/win ui/win/content ui/win/frame \
  io/file

DEPS_FILE=deps.mk

