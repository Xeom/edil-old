srcify = $(addprefix src/, $(addsuffix .c, $(1)))     # Change names into .c paths
objify = $(addprefix obj/, $(addsuffix .o, $(1)))     # Change names into .o paths
depify = $(addprefix dep/, $(addsuffix .d, $(1)))     # Change names into .d paths

L_FLAGS = $(addprefix -l, $(LINKS))    # all the links... (-l*)
W_FLAGS = $(addprefix -W, $(WARNINGS)) # all the warnings... (-W*)
I_FLAGS = $(addprefix -I, $(INCLUDES)) # all the include dirs... (-I*)

DEP_FILES = $(call depify,$(SRC_NAMES))
OBJ_FILES = $(call objify,$(SRC_NAMES))
SRC_FILES = $(call srcify,$(SRC_NAMES))

beginfo="\033[0;32m[\033[1;32mi\033[0;32m]\033[1;32m"
endinfo="\033[0m"
pinfo=echo $(beginfo) $(1) $(endinfo) 1>&2
