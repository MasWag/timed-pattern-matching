PRODUCTS = naive timed-boyer-moore
SRCS = ta_to_ra.cc types.cc naive.cc automaton_operations.cc calcLs.cc 

CXXFLAGS = -std=c++11 -g -O3
LDFLAGS = -lboost_program_options

all: $(PRODUCTS)

LINK.o = $(CXX) $(LDFLAGS) $(TARGET_ARCH)

timed-boyer-moore: timed-boyer-moore.o types.o calcLs.o automaton_operations.o
timed-boyer-moore.o: timed-boyer-moore.hh types.hh ta_to_ra_naive.hh
naive : naive.o
naive.o : naive.hh types.hh
calcLs.o : calcLs.hh types.hh
automaton_operations.o: types.hh

timed-boyer-moore-only-ra.o: timed-boyer-moore.cc timed-boyer-moore.hh types.hh ta_to_ra_naive.hh
	$(CXX) -c $(CXXFLAGS) -DONLY_RA $< -o $@

timed-boyer-moore-only-ra: timed-boyer-moore-only-ra.o types.o calcLs.o automaton_operations.o

clean: 
	$(RM) *.o $(PRODUCTS)
