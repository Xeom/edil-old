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

pinfo=echo "./bracket.sh 35 i;echo '' $(1)" | bash 1>&2
perr= echo "./bracket.sh 31 !;echo '' $(1)" | bash 1>&2
pwarn=echo "./bracket.sh 33 w;echo '' $(1)" | bash 1>&2
psucc=echo "./bracket.sh 32 .;echo '' $(1)" | bash 1>&2
