
INCLUDES = \
	-I $(COMMON_ROOT)/stack	\
	-I $(COMMON_ROOT)/utils	\
	-I $(COMMON_ROOT)/string \
	-I $(COMMON_ROOT)/bit_ops \
	-I $(COMMON_ROOT)/union_find \

OBJS = \
	$(COMMON_ROOT)/utils/multi_thread.o \
	$(COMMON_ROOT)/utils/parser.o \
	$(COMMON_ROOT)/union_find/union_find_test.o \

DEPS = \
	$(COMMON_ROOT)/string/string_api.h \
	$(COMMON_ROOT)/utils/multi_thread.h \
	$(COMMON_ROOT)/utils/scan_utils.h \
	$(COMMON_ROOT)/utils/parser.h \
	$(COMMON_ROOT)/bit_ops/bit_ops.h \
	$(COMMON_ROOT)/union_find/union_find.h \

#order is important to have dependencies work, declare DEPS before include
include $(COMMON_ROOT)/bld/Makefile.common

# cancel implicit program rule
union_find_test: $(OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC) $(OPT) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "--------------------------------------------------"

