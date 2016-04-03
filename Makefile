include conf.mk
include common.mk

$(shell $(call pinfo,"Making dependencies..."))
$(shell make -f deps.mk deps >/dev/null)
$(shell $(call pinfo,"Done"))

include $(DEP_FILES)

obj/%.o: src/%.c
	@$(call pinfo,"Creating object file $@")
	mkdir -p $(@D)
	gcc -c -g $(W_FLAGS) $(I_FLAGS) $< -o $@

clean_dep:
	@$(call pinfo,"Removing dep/*")
	rm -rf dep/*

clean_obj:
	@$(call pinfo,"Removing obj/*")
	rm -rf obj/*

clean: clean_dep clean_obj

obj: $(OBJ_FILES)

.PHONY: clean clean_dep clean_obj
.DEFAULT_GOAL := all
