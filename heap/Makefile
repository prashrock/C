
INCLUDES = \
	-I $(COMMON_ROOT)/heap	\
	-I $(COMMON_ROOT)/utils	\
	-I $(COMMON_ROOT)/bit_ops\

OBJS = \
	$(COMMON_ROOT)/heap/heap_test.o \
	$(COMMON_ROOT)/utils/multi_thread.o \
	$(COMMON_ROOT)/utils/parser.o \

DEPS = \
	$(COMMON_ROOT)/bit_ops/bit_ops.h \
	$(COMMON_ROOT)/heap/heap.h \
	$(COMMON_ROOT)/utils/multi_thread.h \
	$(COMMON_ROOT)/utils/rand_utils.h \
	$(COMMON_ROOT)/utils/compiler_api.h \
	$(COMMON_ROOT)/utils/parser.h \

#order is important to have dependencies work, declare DEPS before include
include $(COMMON_ROOT)/bld/Makefile.common

# cancel implicit program rule
heap_test: $(OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC) $(OPT) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "--------------------------------------------------"

