include conf.mk
include common.mk

$(shell $(call pinfo,"Making dependencies..."))

$(shell make -f deps.mk deps >/dev/null)
$(shell $(call pinfo,"Done"))

$(info $(D_FLAGS))
include $(DEP_FILES)
$(info $(D_FLAGS))

lib.so: $(OBJ_FILES)
	@$(call pinfo,"Linking...")
	$(CC) -g -shared $(W_FLAGS) $(I_FLAGS) $(L_FLAGS) $^ -o lib.so
	@$(call psucc,"Linked into lib.so")
	@$(call pinfo,"Copying lib.so to python...")
	cp $@ python/lib.so
	@$(call psucc,"Copied lib.so to python")

OBJFLAGS= $(D_FLAGS) $(W_FLAGS) $(I_FLAGS) $(F_FLAGS) -fPIC
$(info $(D_FLAGS))
obj/%.o: src/%.c
	@$(call pinfo,"Creating object file $@...")
	mkdir -p $(@D)
	$(CC) -c -g $(OBJFLAGS) --std=$(STD) $< -o $@
	@$(call psucc,"Created object file $@")

test/%.out: test/%.c $(OBJ_FILES)
	@$(call pinfo,"Creating test executable $@...")
	$(CC) -g $(D_FLAGS) $(W_FLAGS) $(L_FLAGS) $(I_FLAGS) --std=$(STD)  $^ -o $@
	@$(call psucc,"Created test executable $@")

clean_dep:
	@$(call pinfo,"Removing dep/*")
	rm -rf dep/*

clean_obj:
	@$(call pinfo,"Removing obj/*")
	rm -rf obj/*

clean_test:
	@$(call pinfo,"Removing test/*/test.out")
	rm test/*/test.out

clean: clean_dep clean_obj clean_test

obj: $(OBJ_FILES)

lib: lib.so

.PHONY: clean clean_dep clean_obj obj lib
.DEFAULT_GOAL := all
