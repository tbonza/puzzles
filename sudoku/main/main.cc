#include "sudoku.h"
#include "sudoku_cli.h"
#include <iostream>
#include <string>

namespace sudoku {

void printHelp() {
  std::cerr
    << "Usage: SudokuSolver <command> <arg>\n\n"
    << "The commands supported by SudokuSolver are:\n\n"
    << "  test              run unit tests\n"
    << "  benchmark         run benchmarks\n"
    << std::endl;
}

int runBenchmark(Benchmarks& benchmark) {
  switch(benchmark) {
    case Benchmarks::all: std::cout << "benchmark all" << std::endl; break;
    default: std::cout << "benchmark all" << std::endl; break;
  }
  return 0;
}

int runTestSuite(Tests& testsuite) {
  switch(testsuite) {
    case Tests::norvig: 
      norvig::internal::tests();
      std::cout << "  PASS - Norvig tests" << std::endl;
      return 0;

    default: std::cout << "test all" << std::endl; break;
  }
  return 0;
}

} // namespace sudoku


int main(int argc, char *argv[]) {

  std::cout << "Sudoku Solver" << std::endl;
  if (argc == 2) {

    std::string command(argv[1]);
    if (command == "test") {
      std::cout << "  Running tests.\n\n";
      auto testsuite = sudoku::Tests::norvig;
      return runTestSuite(testsuite);

    } else if (command == "benchmark") {
      std::cout << "\t" << "Running benchmarks." << std::endl;
      auto benchmark = sudoku::Benchmarks::all;
      return sudoku::runBenchmark(benchmark);

    } else {
      std::cout << "Invalid command. Exiting. " << command << std::endl;
      std::exit(EXIT_FAILURE);
    }

  } else {
    sudoku::printHelp();
    std::exit(EXIT_FAILURE);
  }

  // Run tests


  // Run benchmarks

  return 0;
}
