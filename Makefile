include conf.mk
include common.mk

$(shell $(call pinfo,"Making dependencies..."))
$(shell make -f deps.mk deps >/dev/null)
$(shell $(call pinfo,"Done"))

include $(DEP_FILES)

lib.so: $(OBJ_FILES)
	@$(call pinfo,"Linking...")
	gcc -shared $(W_FLAGS) $(I_FLAGS) $(L_FLAGS) $^ -o lib.so
	cp $@ python/lib.so

obj/%.o: src/%.c
	@$(call pinfo,"Creating object file $@")
	mkdir -p $(@D)
	gcc -c -O2 -g -fPIC $(W_FLAGS) $(I_FLAGS) --std=$(STD) $< -o $@

test/%.out: test/%.c $(OBJ_FILES)
	@$(call pinfo,"Creating test executable $@")
	gcc -g $(W_FLAGS) $(L_FLAGS) $(I_FLAGS) --std=$(STD)  $^ -o $@

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
