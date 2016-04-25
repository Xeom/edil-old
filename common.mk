srcify  = $(addprefix src/,  $(addsuffix .c,      $(1)))     # Change names into .c paths
objify  = $(addprefix obj/,  $(addsuffix .o,      $(1)))     # Change names into .o paths
testify = $(addprefix test/, $(addsuffix /test.c, $(1)))

depify = $(addprefix dep/, $(1:.c=.d))     # Change names into .d paths

L_FLAGS = $(addprefix -l, $(LINKS))    # all the links... (-l*)
W_FLAGS = $(addprefix -W, $(WARNINGS)) # all the warnings... (-W*)
I_FLAGS = $(addprefix -I, $(INCLUDES)) # all the include dirs... (-I*)

OBJ_FILES  = $(call objify,$(SRC_NAMES))
SRC_FILES  = $(call srcify,$(SRC_NAMES))

TEST_FILES = $(call testify,$(TEST_NAMES))

DEP_FILES = $(call depify,$(SRC_FILES)) $(call depify,$(TEST_FILES))

beginfo="\033[0;32m[\033[1;32mi\033[0;32m]\033[1;32m"
endinfo="\033[0m"
pinfo=echo $(beginfo) $(1) $(endinfo) 1>&2
