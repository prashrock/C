
INCLUDES = \
	-I $(COMMON_ROOT)/stack	\
	-I $(COMMON_ROOT)/utils	\

OBJS = \
	$(COMMON_ROOT)/stack/stack_test.o \
	$(COMMON_ROOT)/utils/multi_thread.o \
	$(COMMON_ROOT)/utils/parser.o \

DEPS = \
	$(COMMON_ROOT)/stack/stack.h \
	$(COMMON_ROOT)/stack/dy_stack.h \
	$(COMMON_ROOT)/utils/multi_thread.h \
	$(COMMON_ROOT)/utils/parser.h \

#order is important to have dependencies work, declare DEPS before include
include $(COMMON_ROOT)/bld/Makefile.common

# cancel implicit program rule
stack_test: $(OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC) $(OPT) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "--------------------------------------------------"

