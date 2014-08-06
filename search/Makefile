
INCLUDES = \
	-I $(COMMON_ROOT)/string \
	-I $(COMMON_ROOT)/queue	 \
	-I $(COMMON_ROOT)/stack  \
	-I $(COMMON_ROOT)/utils	 \
	-I $(COMMON_ROOT)/bit_ops \
	-I $(COMMON_ROOT)/sort \

SORT_ROTATED_ARRAY_SEARCH_OBJS = \
	$(COMMON_ROOT)/search/binary_search_sorted_rotated_array.o \
	$(COMMON_ROOT)/utils/parser.o \

DEPS = \
	$(COMMON_ROOT)/utils/compiler_api.h \
	$(COMMON_ROOT)/utils/scan_utils.h \
	$(COMMON_ROOT)/utils/multi_thread.h \
	$(COMMON_ROOT)/utils/time_api.h \
	$(COMMON_ROOT)/queue/dy_queue.h \
	$(COMMON_ROOT)/stack/dy_stack.h \
	$(COMMON_ROOT)/utils/parser.h \
	$(COMMON_ROOT)/string/string_api.h \
	$(COMMON_ROOT)/bit_ops/bitmap.h \
	$(COMMON_ROOT)/bit_ops/bit_ops.h \
	$(COMMON_ROOT)/sort/sort_api.h \
	$(COMMON_ROOT)/sort/selection_sort.h \
	$(COMMON_ROOT)/sort/insertion_sort.h \
	$(COMMON_ROOT)/sort/shell_sort.h \
	$(COMMON_ROOT)/sort/merge_sort.h \

OBJS = $(SORT_ROTATED_ARRAY_SEARCH_OBJS)

#order is important to have dependencies work, declare DEPS before include
include $(COMMON_ROOT)/bld/Makefile.common
LDFLAGS	+= -lrt

# cancel implicit program rule
binary_search_sorted_rotated_array: $(SORT_ROTATED_ARRAY_SEARCH_OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC)  $(OPT) $(CFLAGS) $(LDFLAGS)  -o $@ $^
	@echo "--------------------------------------------------"

