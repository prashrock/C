
INCLUDES = \
	-I $(COMMON_ROOT)/string \
	-I $(COMMON_ROOT)/queue	 \
	-I $(COMMON_ROOT)/stack  \
	-I $(COMMON_ROOT)/utils	 \
	-I $(COMMON_ROOT)/bit_ops \

OBJS = \
	$(COMMON_ROOT)/bit_ops/bit_test.o \
	$(COMMON_ROOT)/utils/multi_thread.o \
	$(COMMON_ROOT)/utils/parser.o \

DEPS = \
	$(COMMON_ROOT)/utils/compiler_api.h \
	$(COMMON_ROOT)/utils/scan_utils.h \
	$(COMMON_ROOT)/utils/multi_thread.h \
	$(COMMON_ROOT)/queue/dy_queue.h \
	$(COMMON_ROOT)/stack/dy_stack.h \
	$(COMMON_ROOT)/utils/parser.h \
	$(COMMON_ROOT)/string/string_api.h \
	$(COMMON_ROOT)/bit_ops/bitmap.h \
	$(COMMON_ROOT)/bit_ops/bit_ops.h \



#order is important to have dependencies work, declare DEPS before include
include $(COMMON_ROOT)/bld/Makefile.common

# cancel implicit program rule
bit_test: $(OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC)  $(OPT) $(CFLAGS) $(LDFLAGS)  -o $@ $^
	@echo "--------------------------------------------------"

