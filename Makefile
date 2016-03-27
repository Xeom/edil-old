#!/usr/bin/make

include conf.mk
# Get configuration

incify=$(addprefix $(INC)/, $(addsuffix .h, $(1))) # Change names into header paths
srcify=$(addprefix $(SRC)/, $(addsuffix .c, $(1))) # Change names into .c paths
objify=$(addprefix $(SRC)/, $(addsuffix .o, $(1))) # Change names into .cs' .o paths

SRC_PATHS=$(call srcify, $(SRC_NAMES)) # All the .c files' paths
OBJ_PATHS=$(call objify, $(SRC_NAMES)) # All the .cs' .o files' paths

L_FLAGS=$(addprefix -l, $(LINKS)) #  # all the links... (-l*)
W_FLAGS=$(addprefix -W, $(WARNINGS)) # all the warnings... (-W*)

OBJ_FLAGS=-I$(INC)/ -g $(W_FLAGS) -fPIC # for compiling objects
DEP_FLAGS=-I$(INC)/ -MM #               # for getting dependency rules

get_rule=$(dir $(1))$(subst \ ,,$(shell $(CC) $(DEP_FLAGS) $(1) -o -)) # Get a dependency rule for a
                                                                   # file($(1)) from the compiler.

append_rule=$(file >>$(DEPS_FILE),$(1)) # Append a rule ($(1)) to the dependecies file.

add_deps=$(call append_rule,$(call get_rule, $(1))) # Add the dependency rule to the depdendency file for a file ($(1))

$(shell rm -f $(DEPS_FILE))                        # Remove the old deps file
$(foreach F, $(SRC_PATHS), $(call add_deps, $(F))) # For every .c file, add dependencies,

#########
# RULES #
#########

include $(DEPS_FILE) # Include dynamic dependencies

# Rule for compiling any .o file
%.o: %.c
	@echo "Compiling $@ ..."
	@$(CC) $(OBJ_FLAGS) -c $< -o $@

objs: $(OBJ_PATHS) # A proxy for compiling all objects

# Rule to compile test.out. Main should be in TEST_PATH
test/%: $(OBJ_PATHS) test/%.c test/test.h
	@echo "Linking into %"
	$(CC) -g $^ $(L_FLAGS) -I$(INC) -o $@

# Compile a shared object and copy into python directory
lib.so: $(OBJ_PATHS)
	@echo "Linking into lib.so"
	@$(CC) -shared $(L_FLAGS) -g $^ -o $@
	@cp $@ python/lib.so

# Cleanup the repo
clean:
	@echo Removing $(DEPS_FILE) and *.o
	@rm -f $(OBJ_PATHS) $(DEPS_FILE)

# Nicer names
test: test.out
lib:  lib.so

.PHONY: test lib clean_tmp clean objs
