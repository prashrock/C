COMMON_ROOT = ~/sample_progs
EXES = stack_test queue_test str_test math_test tree_test sort_test	\
	dyn_subset_sum dyn_fibonacci bit_test					        \
	reservoir_rand_sample dist_elect_test
.PHONY: all clean etags $(EXES)

all: $(EXES) etags

stack_test:
	@$(MAKE) -f $(COMMON_ROOT)/stack/Makefile $@

queue_test:
	@$(MAKE) -f $(COMMON_ROOT)/queue/Makefile $@

str_test:
	@$(MAKE) -f $(COMMON_ROOT)/string/Makefile $@

math_test:
	@$(MAKE) -f $(COMMON_ROOT)/math/Makefile $@

sort_test:
	@$(MAKE) -f $(COMMON_ROOT)/sort/Makefile $@

dyn_subset_sum:
	@$(MAKE) -f $(COMMON_ROOT)/dyn_prog/Makefile $@

dyn_fibonacci:
	@$(MAKE) -f $(COMMON_ROOT)/dyn_prog/Makefile $@

tree_test:
	@$(MAKE) -f $(COMMON_ROOT)/tree/Makefile $@

bit_test:
	@$(MAKE) -f $(COMMON_ROOT)/bit_ops/Makefile $@

reservoir_rand_sample:
	@$(MAKE) -f $(COMMON_ROOT)/rand/Makefile $@

dist_elect_test:
	@$(MAKE) -f $(COMMON_ROOT)/distributed/Makefile $@

clean:
	rm -f $(EXES) TAGS 
	$(MAKE) -C $(COMMON_ROOT)/stack/ clean
	$(MAKE) -C $(COMMON_ROOT)/queue/ clean
	$(MAKE) -C $(COMMON_ROOT)/string/ clean
	$(MAKE) -C $(COMMON_ROOT)/math/ clean
	$(MAKE) -C $(COMMON_ROOT)/dyn_prog/ clean
	$(MAKE) -C $(COMMON_ROOT)/bit_ops/ clean
	$(MAKE) -C $(COMMON_ROOT)/rand/ clean
	$(MAKE) -C $(COMMON_ROOT)/distributed/ clean

etags:
	@echo "--------------------------------------------------"
	rm -f TAGS
	$(FIND_HEADERS_CMD) | etags -
	@echo "--------------------------------------------------"

#create etags file
#first line of find is to exclude whatever we do not need to be 
#indexed by etags, add more like:
#'-or -name $(EXCLUDE_DIR1) -or -name $(EXCLUDE_DIR2)'
#last line after find 'ls' is to add explicit include directories, 
#can add more like:
#'ls $(LIB_DIR)/xulrunner-sdk/include/plugin/np*.h;'
define FIND_HEADERS_CMD
( \
	find $(COMMON_ROOT) '(' -type d -name .svn  ')' -prune  \
	-or -type f '(' -name '*.c' -or -name '*.h' ')' -print; \
	ls /usr/include/*.h; \
)
endef


