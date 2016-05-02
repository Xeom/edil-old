include conf.mk
include common.mk

deps: $(DEP_FILES) $(TEST_FILES)

dep/src/%.d: src/%.c
	@$(call pinfo,"Creating dep file $@")
	mkdir -p $(@D)
	printf $(dir obj/$*) > $@
	gcc -MM $(W_FLAGS) $(I_FLAGS) $< -o - >> $@

dep/test/%/test.d: test/%/test.c
	@$(call pinfo,"Creating dep file $@")
	mkdir -p $(@D)
	printf test/$*/ > $@
	gcc -MM $(W_FLAGS) $(I_FLAGS) $< -o - | sed -e "s/\.o/\.out/" >> $@

.PHONY: deps
