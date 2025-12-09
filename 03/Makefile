.DELETE_ON_ERROR:
.PHONY: clean run
.SILENT: run

CXXFLAGS= $(shell cat compile_flags.txt)

main: puzzle.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f *.o main

run: main
	./$^
