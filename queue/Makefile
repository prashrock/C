
INCLUDES = \
	-I $(COMMON_ROOT)/queue	\
	-I $(COMMON_ROOT)/utils	\
	-I $(COMMON_ROOT)/bit_ops\

OBJS = \
	$(COMMON_ROOT)/queue/queue_test.o \
	$(COMMON_ROOT)/utils/multi_thread.o \
	$(COMMON_ROOT)/utils/parser.o \

DEPS = \
	$(COMMON_ROOT)/bit_ops/bit_ops.h \
	$(COMMON_ROOT)/queue/queue_pow2.h \
	$(COMMON_ROOT)/queue/queue.h \
	$(COMMON_ROOT)/queue/dy_queue.h \
	$(COMMON_ROOT)/utils/multi_thread.h \
	$(COMMON_ROOT)/utils/parser.h \

#order is important to have dependencies work, declare DEPS before include
include $(COMMON_ROOT)/bld/Makefile.common

# cancel implicit program rule
queue_test: $(OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC) $(OPT) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "--------------------------------------------------"

