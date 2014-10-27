
INCLUDES = \
	-I $(COMMON_ROOT)/string \
	-I $(COMMON_ROOT)/queue	 \
	-I $(COMMON_ROOT)/stack  \
	-I $(COMMON_ROOT)/utils	 \
	-I $(COMMON_ROOT)/math	 \
	-I $(COMMON_ROOT)/dyn_prog \

CMPXCHG_OBJS = \
	$(COMMON_ROOT)/micro_benchmarks/cmpxchg.o

RDTSC_OBJS = \
	$(COMMON_ROOT)/micro_benchmarks/rdtsc.o

DEPS = \
	$(COMMON_ROOT)/utils/compiler_api.h \
	$(COMMON_ROOT)/utils/scan_utils.h \
	$(COMMON_ROOT)/utils/print_utils.h \
	$(COMMON_ROOT)/utils/multi_thread.h \
	$(COMMON_ROOT)/utils/parser.h \
	$(COMMON_ROOT)/utils/time_api.h \
	$(COMMON_ROOT)/utils/atomic.h \

OBJS = $(CMPXCHG_OBJS) $(RDTSC_OBJS)

#order is important to have dependencies work, declare DEPS before include
include $(COMMON_ROOT)/bld/Makefile.common
LDFLAGS	+= -lrt
OPT += -O2

# cancel implicit program rule
cmpxchg: $(CMPXCHG_OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC) $(OPT) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "--------------------------------------------------"

readtsc: $(RDTSC_OBJS)
	@echo "--------------------------------------------------"
	@echo $@
	$(CC) $(OPT) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "--------------------------------------------------"

