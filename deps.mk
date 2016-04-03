include conf.mk
include common.mk

deps: $(DEP_FILES)

dep/%.d: src/%.c
	@$(call pinfo,"Creating dep file $@")
	mkdir -p $(@D)
	printf $(dir obj/$*) > $@
	gcc -MM $(W_FLAGS) $(I_FLAGS) $< -o - >> $@

.PHONY: deps
