CXX=g++
CXXFLAGS= -Wall -DNDEBUG -O3 -std=c++11

hash-algorithms:= $(wildcard *.ipp)
hash-bases:= $(basename $(hash-algorithms))
hash:= $(addsuffix .hash, $(hash-bases))
comp:= $(addsuffix .compile, $(hash-bases))

N = 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 #2097152 4194304 8388608 16777216 33554432

OBJS=dshash.o
OBJS+=main.o

HDBRS=dshash.hpp

TEST_OBJS=dshash.o
TEST_OBJS+=dshash_test.o


dshash: $(OBJS) $(HDRS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

dshash_test: $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(TEST_OBJS) -o $@


$(comp): %.compile : %.ipp
	@cp $*.ipp hash.ipp
	$(CXX) $(CXXFLAGS) hotbox.cpp -o $*

$(hash): %.hash : %.compile
	@rm -f $*.log
	for n in $(N) ; do \
	  ./$* books.txt $$n >> $*.log ; \
	  echo -n '.' ; \
	done
	@echo
	@#@rm -f ./a.out 

plot:
	@gnuplot < time.gp
	@epstopdf time.eps

clean:
	- rm  -f core a.out algorithm.i++ time.eps *.o 2>/dev/null
	@rm -f $(OBJS) $(TEST_OBJS) dshash dshash_test

veryclean: clean
	- rm -f *~ *.log time.pdf 2>/dev/null $(sorting-bases)

