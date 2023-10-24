#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

namespace sudoku {

enum class Tests { all, norvig, leetcode, quantum };
int runTestSuite(Tests& testsuite);

} // namespace sudoku

#endif // TEST_RUNNER_H
