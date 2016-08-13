srcify  = $(addprefix src/,  $(addsuffix .c,      $(1))) # Change names into .c paths
objify  = $(addprefix obj/,  $(addsuffix .o,      $(1))) # Change names into .o paths
testify = $(addprefix test/, $(addsuffix /test.c, $(1))) # Change names into c test files

depify = $(addprefix dep/, $(1:.c=.d)) # Change .c paths into .d paths

D_FLAGS = $(addprefix -D, $(DEFINES))
L_FLAGS = $(addprefix -l, $(LINKS))    # all the links...        (-l*)
W_FLAGS = $(addprefix -W, $(WARNINGS)) # all the warnings...     (-W*)
I_FLAGS = $(addprefix -I, $(INCLUDES)) # all the include dirs... (-I*)
F_FLAGS = $(addprefix -f, $(FOPTIONS)) # all the f options...    (-f*)

OBJ_FILES  = $(call objify,$(SRC_NAMES)) # A list of all object file names
SRC_FILES  = $(call srcify,$(SRC_NAMES)) # A list of all source file names

TEST_FILES = $(call testify,$(TEST_NAMES)) # A list of all c test file names

DEP_FILES = $(call depify,$(SRC_FILES)) $(call depify,$(TEST_FILES)) # Depfiles to make for ALL .c files

# Functions for printing logs prettily
pinfo=./misc/plog.sh info $(1) 
perr= ./misc/plog.sh err  $(1)
pwarn=./misc/plog.sh warn $(1)
psucc=./misc/plog.sh info $(1)
