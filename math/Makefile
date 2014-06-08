
INCLUDES = \
	-I $(COMMON_ROOT)/string \
	-I $(COMMON_ROOT)/queue	 \
	-I $(COMMON_ROOT)/stack  \
	-I $(COMMON_ROOT)/utils	 \
	-I $(COMMON_ROOT)/math	 \

MATH_OBJS = \
	$(COMMON_ROOT)/math/math_test.o \
	$(COMMON_ROOT)/utils/multi_thread.o \
	$(COMMON_ROOT)/utils/parser.o \

SUBSUM_OBJS = \
	$(COMMON_ROOT)/math/subset_sum.o \
	$(COMMON_ROOT)/utils/parser.o \

DEPS = \
	$(COMMON_ROOT)/utils/compiler_api.h \
	$(COMMON_ROOT)/utils/scan_utils.h \
	$(COMMON_ROOT)/utils/multi_thread.h \
	$(COMMON_ROOT)/stack/dy_stack.h \
	$(COMMON_ROOT)/queue/dy_queue.h \
	$(COMMON_ROOT)/utils/parser.h \
	$(COMMON_ROOT)/math/math_fact.h \
	$(COMMON_ROOT)/math/math_misc.h \
	$(COMMON_ROOT)/math/math_prime.h \

#order is important to have dependencies work, declare DEPS before include
include $(COMMON_ROOT)/bld/Makefile.common

# cancel implicit program rule
math_test: $(MATH_OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC) $(OPT) $(CFLAGS) $(LDFLAGS) -lm -o $@ $^
	@echo "--------------------------------------------------"

subset_sum: $(SUBSUM_OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC) $(OPT) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "--------------------------------------------------"

