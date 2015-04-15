
INCLUDES = \
	-I $(COMMON_ROOT)/queue	\
	-I $(COMMON_ROOT)/stack	\
	-I $(COMMON_ROOT)/utils	\
	-I $(COMMON_ROOT)/tree	\
	-I $(COMMON_ROOT)/string\

TREE_TEST_OBJS = \
	$(COMMON_ROOT)/tree/tree_test.o \
	$(COMMON_ROOT)/utils/multi_thread.o \
	$(COMMON_ROOT)/utils/parser.o \

BIT_TREE_OBJS = \
	$(COMMON_ROOT)/tree/bit_tree_test.o \
	$(COMMON_ROOT)/utils/multi_thread.o \
	$(COMMON_ROOT)/utils/parser.o \

DEPS = \
	$(COMMON_ROOT)/utils/compiler_api.h \
	$(COMMON_ROOT)/utils/rand_utils.h \
	$(COMMON_ROOT)/queue/dy_queue.h \
	$(COMMON_ROOT)/stack/dy_stack.h \
	$(COMMON_ROOT)/utils/multi_thread.h \
	$(COMMON_ROOT)/utils/parser.h \
	$(COMMON_ROOT)/string/string_api.h \
	$(COMMON_ROOT)/tree/bst.h \

OBJS = $(TREE_TEST_OBJS) $(BIT_TREE_OBJS)

#order is important to have dependencies work, declare DEPS before include
include $(COMMON_ROOT)/bld/Makefile.common

# cancel implicit program rule
tree_test: $(TREE_TEST_OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC) $(OPT) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "--------------------------------------------------"

bit_tree_test: $(BIT_TREE_OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC) $(OPT) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "--------------------------------------------------"