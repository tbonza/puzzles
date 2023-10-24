#include "test_runner.h"
#include "sudoku.h"

namespace sudoku {
  
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
  std::cout << "  Running tests.\n\n";
  auto testsuite = sudoku::Tests::norvig;
  return runTestSuite(testsuite);
}
