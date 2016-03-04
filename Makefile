#!/usr/bin/make

all_proxy: all

VERSION=0.0.1

# Toolchain programs.
CC=gcc
LD=ld
AR=ar
LN=ln

# Directories.
SRC=src
INC=src/include

WARNINGS=all extra no-unused-parameter pedantic missing-prototypes fatal-errors format

W_FLAGS=$(addprefix -W, $(WARNINGS))

# Source and header files.
SRC_OBJS=line vec err cursor ui wintree wincont
INC_OBJS=line vec err cursor ui wintree wincont

OBJ_PATHS=$(addprefix $(SRC)/, $(addsuffix .o, $(SRC_OBJS)))
INC_PATHS=$(addprefix $(INC)/, $(addsuffix .h, $(INC_OBJS)))

# Compiler flags.
CCFLAGS=-I$(INC) -g $(W_FLAGS)

# Default rule for compiling object files.
%.o: %.c $(INC)
	$(CC) $(CCFLAGS) -fPIC -c $< -o $@

# Main rule.
all: lib.so
test: $(OBJ_PATHS) $(SRC)/test.c
	$(CC) -g $^ -lncurses -o $@

# Lib shit yo
lib.so: $(OBJ_PATHS)
	$(CC) -shared -fPIC -g $^ -o $@
	cp $@ python/lib.so

# Clean repo.
clean:
	rm -f $(OBJ_PATHS) test

.PHONY: all_proxy all clean
