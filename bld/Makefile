COMMON_ROOT = $(shell pwd)/..
EXES = stack_test queue_test str_test math_test sort_test heap_test \
	tree_test bit_tree_test                                         \
	dyn_subset_sum dyn_fibonacci dyn_str_min_edit_dist bit_test     \
	dyn_maximum_subarray                                            \
	div_conq_stock_min_max                                          \
	reservoir_rand_sample dist_elect_test ipc_socket_test           \
	search_test union_find_test cmpxchg readtsc mmap_test
.PHONY: all clean etags $(EXES)

all: $(EXES) etags

stack_test:
	@$(MAKE) -f $(COMMON_ROOT)/stack/Makefile COMMON_ROOT="$(COMMON_ROOT)"  $@

queue_test:
	@$(MAKE) -f $(COMMON_ROOT)/queue/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

heap_test:
	@$(MAKE) -f $(COMMON_ROOT)/heap/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

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

dyn_str_min_edit_dist:
	@$(MAKE) -f $(COMMON_ROOT)/dyn_prog/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

dyn_maximum_subarray:
	@$(MAKE) -f $(COMMON_ROOT)/dyn_prog/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

div_conq_stock_min_max:
	@$(MAKE) -f $(COMMON_ROOT)/divide_conquer/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

tree_test:
	@$(MAKE) -f $(COMMON_ROOT)/tree/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

bit_tree_test:
	@$(MAKE) -f $(COMMON_ROOT)/tree/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

bit_test:
	@$(MAKE) -f $(COMMON_ROOT)/bit_ops/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

reservoir_rand_sample:
	@$(MAKE) -f $(COMMON_ROOT)/rand/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

dist_elect_test:
	@$(MAKE) -f $(COMMON_ROOT)/distributed/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

ipc_socket_test:
	@$(MAKE) -f $(COMMON_ROOT)/IPC/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

search_test:
	@$(MAKE) -f $(COMMON_ROOT)/search/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

union_find_test:
	@$(MAKE) -f $(COMMON_ROOT)/union_find/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

cmpxchg:
	@$(MAKE) -f $(COMMON_ROOT)/micro_benchmarks/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

readtsc:
	@$(MAKE) -f $(COMMON_ROOT)/micro_benchmarks/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

mmap_test:
	@$(MAKE) -f $(COMMON_ROOT)/micro_benchmarks/Makefile COMMON_ROOT="$(COMMON_ROOT)" $@

clean:
	rm -f $(EXES) TAGS 
	$(MAKE) -C $(COMMON_ROOT)/stack/       COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/queue/       COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/heap/        COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/string/      COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/math/        COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/sort/        COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/dyn_prog/    COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/tree/        COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/bit_ops/     COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/rand/        COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/distributed/ COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/IPC/         COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/search/      COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/union_find/  COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/divide_conquer/   COMMON_ROOT="$(COMMON_ROOT)" clean
	$(MAKE) -C $(COMMON_ROOT)/micro_benchmarks/ COMMON_ROOT="$(COMMON_ROOT)" clean

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

