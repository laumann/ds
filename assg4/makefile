CXX=g++-4.7
CXXFLAGS=-O3 -Wall -std=c++11

sorting-algorithms:= $(wildcard *.i++)
sorting-bases:= $(basename $(sorting-algorithms))
sort:= $(addsuffix .sort, $(sorting-bases)) 
comp:= $(addsuffix .compile, $(sorting-bases)) 

N = 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608 16777216 33554432

$(comp): %.compile: %.i++
	@cp $*.i++ algorithm.i++
	$(CXX) $(CXXFLAGS) sort-driver.c++
	@rm algorithm.i++

$(sort): %.sort : %.compile
	@rm -f $*.log
	@for n in $(N) ; do \
	  ./a.out $$n >> $*.log ; \
	  echo -n '.' ; \
	done
	@#@rm -f ./a.out 

plot:
	@gnuplot < time.gp
	@epstopdf time.eps

clean:
	- rm  -f core a.out algorithm.i++ time.eps *.o 2>/dev/null

veryclean: clean
	- rm -f *~ *.log time.pdf 2>/dev/null
