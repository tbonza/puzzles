CC = clang++
CFLAGS = -std=c++11 -Wall -g -I ../include

all: Sudoku

builddir:
	mkdir -p build

clean: builddir
	rm -r build

Sudoku: builddir
	pushd build && \
	$(CC) $(CFLAGS) \
	../lib/sudoku_solver.cc ../lib/sudoku_solver_test.cc \
	../lib/sudoku_leetcode.cc \
	../main/main.cc \
	-o SudokuSolver

benchmark: builddir
	pushd build && \
	$(CC) $(CFLAGS) -lbenchmark -lpthread \
	../lib/sudoku_solver.cc \
	../benchmark/hello.cc \
	-o BenchmarkSudokuSolver
