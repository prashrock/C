
INCLUDES = \
	-I $(COMMON_ROOT)/string \
	-I $(COMMON_ROOT)/queue	 \
	-I $(COMMON_ROOT)/stack  \
	-I $(COMMON_ROOT)/utils	 \
	-I $(COMMON_ROOT)/math	 \
	-I $(COMMON_ROOT)/dyn_prog \


DYN_FIB_OBJS = \
	$(COMMON_ROOT)/dyn_prog/dyn_fibonacci.o \
	$(COMMON_ROOT)/utils/parser.o \


DYN_MAX_SUBARRAY_OBJS = \
	$(COMMON_ROOT)/dyn_prog/dyn_maximum_subarray.o \
	$(COMMON_ROOT)/utils/parser.o \

DEPS = \
	$(COMMON_ROOT)/utils/compiler_api.h \
	$(COMMON_ROOT)/utils/scan_utils.h \
	$(COMMON_ROOT)/utils/print_utils.h \
	$(COMMON_ROOT)/utils/multi_thread.h \
	$(COMMON_ROOT)/utils/rand_utils.h \
	$(COMMON_ROOT)/stack/dy_stack.h \
	$(COMMON_ROOT)/queue/dy_queue.h \
	$(COMMON_ROOT)/utils/parser.h \
	$(COMMON_ROOT)/math/math_fact.h \
	$(COMMON_ROOT)/math/math_misc.h \
	$(COMMON_ROOT)/math/math_prime.h \

OBJS = $(DYN_SUBSUM_OBJS) $(DYN_FIB_OBJS) $(DYN_STR_MIN_EDIT_OBJS) \
	$(DYN_MAX_SUBARRAY_OBJS)

#order is important to have dependencies work, declare DEPS before include
include $(COMMON_ROOT)/bld/Makefile.common

# cancel implicit program rule

dyn_fibonacci: $(DYN_FIB_OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC) $(OPT) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "--------------------------------------------------"

dyn_maximum_subarray: $(DYN_MAX_SUBARRAY_OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC) $(OPT) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "--------------------------------------------------"
