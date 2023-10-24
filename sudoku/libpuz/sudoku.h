
#ifndef SUDOKU_NORVIG_H
#define SUDOKU_NORVIG_H

#include <cassert>
#include <iostream>
#include <limits>
#include <set>
#include <unordered_set> // deprecate
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace sudoku {
namespace norvig {

/**
 * Helper functions.
 *
 * These are converted from what Norvig did in Python.
 * https://norvig.com/sudoku.html
 */

const std::set<char> DIGITS = {
  '1','2','3','4','5','6','7','8','9'};
const std::set<char> ROWS = {'A','B','C','D','E','F','G','H','I'};
const std::set<char> COLS = DIGITS;
const std::vector<std::set<char>> ROW_BLOCKS = { 
  {'A','B','C'}, {'D','E','F'}, {'G','H','I'},
};
const std::vector<std::set<char>> COL_BLOCKS = { 
  {'1','2','3'}, {'4','5','6'}, {'7','8','9'},
};

/**
 * Sudoku IO
 * Functions responsible for Input/Output of Sudoku puzzle. We have inputs
 * from the Norvig blog post as well as a LeetCode problem.
 */

// Convert grid into map of {square: char} with '0' or '.' for empties.
void gridValues(const std::string& grid, 
    std::unordered_map<std::string,std::set<char>>& values,
    std::vector<std::string>& squares);

// String formatting not in stdlib
std::string centerFmt(std::set<char>& cands, int width);

// Display these values as a 2-D grid.
//void display(std::unordered_map<std::string,std::string>& values);
void display(std::unordered_map<std::string,std::set<char>>& values, 
    std::vector<std::string> squares);

/**
 * Sudoku solver using constraint propagation and backtracking.
 *
 * Norvig recommended starting with constraint propagation, noticed
 * this baseline strategy wasn't sufficient for harder problems, then
 * implemented backtracking search.
 */
void cross(const std::set<char>& rows, 
           const std::set<char>& cols,
           std::vector<std::string>& product);

// Each square has exactly 3 units and 20 peers.
bool findPeersUnits(std::unordered_map<std::string,
    std::vector<std::unordered_set<std::string>>>& units,
    std::unordered_map<std::string,
    std::unordered_set<std::string>>& peers,
    std::vector<std::string>& squares);

// Eliminate all the other values (except d) from values[s] and 
// propagate. Return values, except return False if a 
// contradiction is detected.
bool assign(std::unordered_map<std::string,std::set<char>>& values,
            const std::string& square, char d,
            std::unordered_map<std::string,
              std::vector<std::unordered_set<std::string>>>& units,
            std::unordered_map<std::string,
              std::unordered_set<std::string>>& peers);

// Constraint propagation
//
//  (1) If a square has only one possible value, then 
//      eliminate that value from the square's peers.
//  (2) If a unit has only one possible place for a value,
//      then put the value there.
//
// Updates to a square may in turn cause further updates to its 
// peers, and the peers of those peers, and so on. This process is
// called constraint propagation. It turns out that the fundamental
// operation is not assigning a value, but rather eliminating one of the
// possible values for a square.

// Eliminate d from values[s]; propagate when values or places <= 2.
// Return values, except return false if a contradiction is detected.
bool eliminate(std::unordered_map<std::string,std::set<char>>& values,
            const std::string& square, char d,
            std::unordered_map<std::string,
              std::vector<std::unordered_set<std::string>>>& units,
            std::unordered_map<std::string,
              std::unordered_set<std::string>>& peers);

bool some(const std::string& minSquare, 
    std::unordered_map<std::string,std::set<char>>& values,
    std::vector<std::string>& squares,
          std::unordered_map<std::string,
      std::vector<std::unordered_set<std::string>>>& units,
    std::unordered_map<std::string,
      std::unordered_set<std::string>>& peers);

// Using depth first search and propagation, try all possible values.
//
// Intuition is to prune a tree which defines a search space. By starting
// with the square containing the minimum number of digits, we move to the
// next digit by finding which candidate can be assigned. Each tree branch
// must allocate memory independently of the other branches. We use
// depth-first-search to prune one branch at a time.
bool search(bool status, std::unordered_map<std::string,
    std::set<char>>& values,
            std::vector<std::string>& squares,
            std::unordered_map<std::string,
              std::vector<std::unordered_set<std::string>>>& units,
            std::unordered_map<std::string,
              std::unordered_set<std::string>>& peers);

// Norvig solvers
bool solveWithConstraints(std::string& grid, bool verbose);

bool solveWithSearch(std::string& grid, bool verbose);

namespace internal {

/**
 * Norvig Sudoku Examples
 *
 * Norvig pickedd the first example from a list of easy puzzles from the
 * fine Project Euler Sukoku problem. He chose other specific examples
 * as well.
 */
void easyEulerSudokuExamples();

/**
 * Testing Helper functions
 */

// Replicating Novig Sudoku solver; 'units test'
//
// https://norvig.com/sudoku.html
// Run tests and examples.
void tests();
void examples();

} // namespace internal
} // namespace norvig


} // namespace sudoku

#endif // SUDOKU_NORVIG_H

