COMMON_ROOT = $(shell pwd)/..
EXES = stack_test queue_test str_test math_test tree_test sort_test	\
	dyn_subset_sum dyn_fibonacci bit_test					        \
	reservoir_rand_sample dist_elect_test
.PHONY: all clean etags $(EXES)

all: $(EXES) etags

stack_test:
	@$(MAKE) -f $(COMMON_ROOT)/stack/Makefile COMMON_ROOT="$(COMMON_ROOT)"  $@

queue_test:
	@$(MAKE) -f $(COMMON_ROOT)/queue/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

str_test:
	@$(MAKE) -f $(COMMON_ROOT)/string/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

math_test:
	@$(MAKE) -f $(COMMON_ROOT)/math/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

sort_test:
	@$(MAKE) -f $(COMMON_ROOT)/sort/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

dyn_subset_sum:
	@$(MAKE) -f $(COMMON_ROOT)/dyn_prog/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

dyn_fibonacci:
	@$(MAKE) -f $(COMMON_ROOT)/dyn_prog/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

tree_test:
	@$(MAKE) -f $(COMMON_ROOT)/tree/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

bit_test:
	@$(MAKE) -f $(COMMON_ROOT)/bit_ops/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

reservoir_rand_sample:
	@$(MAKE) -f $(COMMON_ROOT)/rand/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

dist_elect_test:
	@$(MAKE) -f $(COMMON_ROOT)/distributed/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

clean:
	rm -f $(EXES) TAGS 
	$(MAKE) -C $(COMMON_ROOT)/stack/       COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/queue/       COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/string/      COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/math/        COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/sort/        COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/dyn_prog/    COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/tree/        COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/bit_ops/     COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/rand/        COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/distributed/ COMMON_ROOT="$(COMMON_ROOT)" clean

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


