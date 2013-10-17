CXX=g++-4.7
CXXFLAGS= -Wall -DNDEBUG -O3 -std=c++11

sorting-algorithms:= $(wildcard *.i++)
sorting-bases:= $(basename $(sorting-algorithms))
sort:= $(addsuffix .sort, $(sorting-bases)) 
comp:= $(addsuffix .compile, $(sorting-bases)) 

N = 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 16777216 33554432

stl_hash: stl_hash.c++
	$(CXX) $(CXXFLAGS) $< -o $@

$(comp): %.compile: %.i++
	@cp $*.i++ algorithm.i++
	$(CXX) $(CXXFLAGS) sort-driver.c++ -o $*
	@rm algorithm.i++

$(sort): %.sort : %.compile
	@rm -f $*.log
	for n in $(N) ; do \
	  ./$* $$n >> $*.log ; \
	  echo -n '.' ; \
	done
	@echo
	@#@rm -f ./a.out 

compare-dump:
	@cp cmov_qsort.i++ algorithm.i++
	$(CXX) $(CXXFLAGS) sort-driver.c++ -o qsort.o -c
	$(CXX) $(CXXFLAGS) sort-driver.c++ -o cmov_qsort.o -c
	@objdump -d qsort.o > qsort.dump
	@objdump -d cmov_qsort.o > cmov_qsort.dump
	@diff -Naur qsort.dump cmov_qsort.dump |colordiff |less -r

plot:
	@gnuplot < time.gp
	@epstopdf time.eps

clean:
	- rm  -f core a.out algorithm.i++ time.eps *.o 2>/dev/null

veryclean: clean
	- rm -f *~ *.log time.pdf 2>/dev/null $(sorting-bases)

