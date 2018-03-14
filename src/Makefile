PRODUCTS = naive timedFJS timedBM timedBM_region naive_online_lazy timedFJS_lazy
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:%.cc=%.o)
HEADS = $(wildcard *.hh)

CXXFLAGS = -std=c++11 -march=native
CXX_DEBUG_FLAGS = -g -O0
CXX_RELEASE_FLAGS = -O3 -DPRODUCT -flto
LDFLAGS = -march=native
LDLIBS = -lboost_program_options
LD_RELEASE_FLAGS = -O3 -DPRODUCT -flto


.PHONY: debug
debug: CXXFLAGS+=$(CXX_DEBUG_FLAGS)
debug: all


.PHONY: release
release: CXXFLAGS+=$(CXX_RELEASE_FLAGS)
release: LDFLAGS+=$(LD_RELEASE_FLAGS)
release: all

.PHONY: all clean depend 

all: Makefile.depend $(PRODUCTS)

LINK.o = $(CXX) $(LDFLAGS) $(TARGET_ARCH)

naive: naive.o
naive_online_lazy: naive_online_lazy.o automaton_operations.o types.o
timedFJS: timedFJS.o automaton_operations.o types.o
timedFJS_lazy: timedFJS_lazy.o automaton_operations.o types.o
timedBM: timedBM.o automaton_operations.o types.o
timedBM_region: timedBM_region.o automaton_operations.o types.o

timedFJS_lazy.o: timedFJS.cc timedFJS.hh
	$(COMPILE.cc) -D LAZY_READ $(OUTPUT_OPTION) $<

Makefile.depend: $(SRCS)
	$(CXX) -MM -MG $(SRCS) > Makefile.depend

naive_online_lazy.o: naive_online.cc naive_online.hh
	$(COMPILE.cc) -D LAZY_READ $(OUTPUT_OPTION) $<

clean:
	$(RM) *.o $(PRODUCTS) Makefile.depend

-include Makefile.depend
