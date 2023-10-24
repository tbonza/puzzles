#include <benchmark/benchmark.h>
#include "libpuz/sudoku.h"
#include <cassert>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

class NorvigFixture : public benchmark::Fixture {
 public:
  std::vector<std::string> hard_top95;
  std::vector<std::string> hardest;
  int high;
  int hhigh;
  int low = 0;
  std::uint32_t seed;

  void SetUp(const ::benchmark::State& state) {

    const std::string hard_top95_file_path = 
      "resources/norvig_top95_sudoku.txt";

    std::ifstream infile(hard_top95_file_path);
    std::string line;
    while (std::getline(infile, line)) {
      hard_top95.push_back(line);
    }

    high = hard_top95.size() - 1;
    seed = 42;

    const std::string hardest_file_path =
      "resources/norvig_hardest.txt";
    std::string hline;
    std::ifstream hardest_infile(hardest_file_path);
    while (std::getline(hardest_infile, hline)) {
      hardest.push_back(hline);
    }

    hhigh = hardest.size() - 1;
  }

};

static void BM_StringCreation(benchmark::State& state) {
    for (auto _ : state)
          std::string empty_string;
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation);

// Define another benchmark
BENCHMARK_F(NorvigFixture, BM_SolveWithSearch_Hard)(benchmark::State& state) {
  std::mt19937 generator (seed);
  std::uniform_int_distribution<int> distribution(low,high);

  bool status = false;
  for (auto _ : state) {

    state.PauseTiming();
    int i = distribution(generator);
    std::string grid = hard_top95.at(i);
    state.ResumeTiming();

    status = sudoku::norvig::solveWithSearch(grid, false);
    assert(status);
  }
}

// Define another benchmark
BENCHMARK_F(NorvigFixture, BM_SolveWithSearch_Hardest)(benchmark::State& state) {
  std::mt19937 generator (seed);
  std::uniform_int_distribution<int> distribution(low,hhigh);

  bool status = false;
  for (auto _ : state) {

    state.PauseTiming();
    int i = distribution(generator);
    std::string grid = hardest.at(i);
    state.ResumeTiming();

    status = sudoku::norvig::solveWithSearch(grid, false);
    assert(status);
  }
}

BENCHMARK_MAIN();
