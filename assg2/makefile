CXX=g++-4.7
CXXFLAGS=-O3 -Wall -DNDEBUG -std=c++11

set-implementations:= $(wildcard *.i++)
set-bases:= $(basename $(set-implementations))
insert:= $(addsuffix .insert, $(set-bases)) 

N = 1024 32768 1048576

$(insert): %.insert : %.i++
	@cp $*.i++ data-structure.i++
	$(CXX) $(CXXFLAGS) insert-driver.c++
	@for n in $(N) ; do \
	  ./a.out $$n ; \
	done; \
	rm -f ./a.out 

23tree:
	$(CXX) $(CXXFLAGS) 23tree.c++

debug:
	@echo $(set-implementations)
	@echo $(set-bases)
	@echo $(insert)

clean:
	- rm  -f core a.out data-structure.i++ *.o 2>/dev/null

veryclean: clean
	- rm -f *~ 2>/dev/null
