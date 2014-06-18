
INCLUDES = \
	-I $(COMMON_ROOT)/string \
	-I $(COMMON_ROOT)/queue	 \
	-I $(COMMON_ROOT)/stack  \
	-I $(COMMON_ROOT)/utils	 \
	-I $(COMMON_ROOT)/math	 \

IPC_SOCKET_OBJS = \
	$(COMMON_ROOT)/IPC/ipc_socket_test.o \

DEPS = \
	$(COMMON_ROOT)/IPC/ipc_unix_stream_socket_api.h \
	$(COMMON_ROOT)/utils/compiler_api.h \
	$(COMMON_ROOT)/utils/scan_utils.h \
	$(COMMON_ROOT)/utils/multi_thread.h \
	$(COMMON_ROOT)/stack/dy_stack.h \
	$(COMMON_ROOT)/queue/dy_queue.h \
	$(COMMON_ROOT)/utils/parser.h \

OBJS = $(IPC_SOCKET_OBJS)

#order is important to have dependencies work, declare DEPS before include
include $(COMMON_ROOT)/bld/Makefile.common

# cancel implicit program rule
ipc_socket_test: $(IPC_SOCKET_OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC) $(OPT) $(CFLAGS) $(LDFLAGS) -lm -o $@ $^
	@echo "--------------------------------------------------"
