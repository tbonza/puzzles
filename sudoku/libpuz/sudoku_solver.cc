
#include "sudoku.h"

namespace sudoku {
namespace norvig {

/** 
 * Sudoku IO
 */

void gridValues(const std::string& grid, 
    std::unordered_map<std::string,std::set<char>>& values,
    std::vector<std::string>& squares) {

  if (grid.size() != squares.size()) {
    std::cerr << "Invalid grid size. " << grid.size() 
              << " Exiting." << std::endl;
    std::exit(EXIT_FAILURE);
  }

  for (int i = 0; i < squares.size(); i++) {
    values[squares[i]].insert(grid[i]);
  }
}

std::string centerFmt(std::set<char>& cands, int width) {
  int leftPad = 0, rightPad = 0;
  if (width > cands.size()) {
    int diff = width - cands.size();
    leftPad = diff/2; 
    rightPad = diff - leftPad;
  }

  std::stringstream ss;
  ss << std::string(leftPad, ' ');
  for (auto i = cands.begin(); i != cands.end(); ++i) {
    ss << *i;
  }
  if (cands.size() == 0)
    ss << '.';

  ss << std::string(rightPad, ' ');
  return ss.str();
}

void display(std::unordered_map<std::string,std::set<char>>& values, 
    std::vector<std::string> squares) {

  // Find cell width
  int width = 0;
  for (const auto& v : values) {
    if (width < v.second.size())
      width = v.second.size();
  }
  width++;

  // Create line for display
  std::stringstream segment;
  for (int i=0; i < width * 3; i++) {
    segment << "-";
  }
  std::stringstream line;
  line << segment.str();
  for (int i=1; i < 3; i++) {
    line << "+" << segment.str();
  }

  // Display cell candidates with centered format.
  for (auto r = ROWS.begin(); r != ROWS.end(); ++r) {
    for (auto c = COLS.begin(); c != COLS.end(); ++c) {
      std::stringstream square;
      square << *r << *c;

      std::stringstream cands;
      cands << centerFmt(values[square.str()], width);

      
      if (*c == '3' || *c == '6')
        cands << '|';

      std::cout << cands.str();
    }
    std::cout << std::endl;
    if (*r == 'C' || *r == 'F')
      std::cout << line.str() << std::endl;
  }
  std::cout << std::endl;
}

/**
 * Sudoku Solver
 */

void cross(const std::set<char>& rows, 
           const std::set<char>& cols,
           std::vector<std::string>& product) {

  for (auto i = rows.begin(); i != rows.end(); ++i) {
    for (auto j = cols.begin(); j != cols.end(); ++j) {
      std::stringstream ss;
      ss << *i << *j;
      product.push_back(ss.str());
    }
  }
}

bool findPeersUnits(std::unordered_map<std::string,
    std::vector<std::unordered_set<std::string>>>& units,
    std::unordered_map<std::string,
    std::unordered_set<std::string>>& peers,
    std::vector<std::string>& squares) {

  // Find unit names for all units.

  std::vector<std::string> product;
  std::vector<std::unordered_set<std::string>> unitlist;

  for (auto c = COLS.begin(); c != COLS.end(); ++c) {
    const std::set<char> hasc = { *c };
    cross(ROWS, hasc, product);
    unitlist.emplace_back(
        std::unordered_set<std::string>(product.begin(), 
                                        product.end()));
    product.clear();
  }

  for (auto r = ROWS.begin(); r != ROWS.end(); ++r) {
    const std::set<char> hasr = { *r };
    cross(hasr, COLS, product);
    unitlist.emplace_back(
        std::unordered_set<std::string>(product.begin(), 
                                        product.end()));
    product.clear();
  }

  for (const auto& rb : ROW_BLOCKS) {
    for (const auto& cb : COL_BLOCKS) {
      cross(rb, cb, product);
      unitlist.emplace_back(
        std::unordered_set<std::string>(product.begin(), 
                                        product.end()));
      product.clear();
    }
  }

  // Find peers and units for each square.
  for (const std::string& square : squares) {
    units[square] = std::vector<std::unordered_set<std::string>>();
    peers[square] = std::unordered_set<std::string>();
    for (const auto& u : unitlist) {
      if (u.find(square) != u.end()) {
        units[square].push_back(u);

        for (const auto& p : u) {
          if (square != p) {
            peers[square].insert(p);
          }
        }
      }
    }
  }
  return true;
}

bool assign(std::unordered_map<std::string,std::set<char>>& values,
            const std::string& square, char d,
            std::unordered_map<std::string,
              std::vector<std::unordered_set<std::string>>>& units,
            std::unordered_map<std::string,
              std::unordered_set<std::string>>& peers) {

  std::set<char> otherValues = values[square];
  if (otherValues.find(d) != otherValues.end())
    otherValues.erase(d); // erase d if exists

  for (const auto& d2 : otherValues) {
    if (!eliminate(values, square, d2, units, peers))
      return false;
  }
  return true;
}

bool eliminate(std::unordered_map<std::string,std::set<char>>& values,
            const std::string& square, char d,
            std::unordered_map<std::string,
              std::vector<std::unordered_set<std::string>>>& units,
            std::unordered_map<std::string,
              std::unordered_set<std::string>>& peers) {

  // Eliminate digit from possible values for square.
  if (values[square].find(d) == values[square].end())
    return true; // already elminated
  else
    values[square].erase(d);

  // (1) If a square s is reduced to one value d2, 
  //     than eleminate d2 from the peers.
  if (values[square].size() == 0)
    return false; // Contradiction: removed last value
  else if (values[square].size() == 1) {
    char d2 = *values[square].begin();
    for (const std::string& s2 : peers[square]) {
      if (!eliminate(values, s2, d2, units, peers))
        return false;
    }
  }

  // (2) If a unit u is reduced to only one place for a value d,
  //     then put it there.
  for (const auto& unit : units[square]) { 

    std::vector<std::string> dplaces;
    for (const auto& square : unit) {
      if (values[square].find(d) != values[square].end())
        dplaces.push_back(square);
    }

    if (dplaces.size() == 0)
      return false; // Contradiction: no places for this value

    else if (dplaces.size() == 1) {
      // d can only be in one place in unit; assign it there
      if(!assign(values, dplaces.back(), d, units, peers))
        return false;
    }
  }
  return true; // Solution placed in `values`.
}

bool some(const std::string& minSquare, 
    std::unordered_map<std::string,std::set<char>>& values,
    std::vector<std::string>& squares,
          std::unordered_map<std::string,
      std::vector<std::unordered_set<std::string>>>& units,
    std::unordered_map<std::string,
      std::unordered_set<std::string>>& peers) {


  for (const char d : values[minSquare]) {

    // Copy of values for assignment check.
    std::unordered_map<std::string,std::set<char>> tmp;
    for (const auto& v : values) {
      tmp[v.first] = v.second;
    } 

    if (search(assign(tmp, minSquare, d, units, peers), tmp, 
          squares, units, peers)) {

      // Copy from temp back into values
      for (const auto& t : tmp) {
        values[t.first] = t.second;
      }
      return true;
    }
  }
  return false;
}

bool search(bool status,
    std::unordered_map<std::string,std::set<char>>& values,
    std::vector<std::string>& squares,
    std::unordered_map<std::string,
      std::vector<std::unordered_set<std::string>>>& units,
    std::unordered_map<std::string,
      std::unordered_set<std::string>>& peers) {

  if (!status)
    return false; // Failed earlier.

  int count = 0;
  for (const auto& square : squares) {
    if (values[square].size() == 1)
      count += 1;
  }
  if (count == values.size())
    return true; // Solved!

  // Choose the unfilled square s with the fewest possibilities.
  int idx = -1;
  int minPossible = std::numeric_limits<int>::max();
  for (int i = 0; i < squares.size(); i++) {
    int nCands = values[squares[i]].size();
    if (nCands > 1 && nCands < minPossible) {
      minPossible = nCands;
      idx = i;
    }
  }
  const std::string minSquare = squares[idx];

  // Return some value from Search
  return some(minSquare, values, squares, units, peers);
}

bool solveWithConstraints(std::string& grid, bool verbose) {

  // Identify squares
  std::vector<std::string> squares;
  cross(ROWS, COLS, squares);

  // Identify peers and units
  std::unordered_map<std::string,std::unordered_set<std::string>> peers;
  std::unordered_map<std::string,std::vector<
    std::unordered_set<std::string>>> units;

  findPeersUnits(units, peers, squares);

  // To start, every square can be any digit; 
  // then assign values from the grid.
  std::unordered_map<std::string,std::set<char>> values;
  for (const auto& square : squares) {
    values[square] = DIGITS;
  }

  std::unordered_map<std::string,std::set<char>> gvalues;
  gridValues(grid, gvalues, squares);

  if (verbose) {
    std::cout << "Sudoku Input" << std::endl;
    display(gvalues, squares);
  }

  for (const auto& cell : gvalues) {
    std::string square = cell.first;
    char d = *cell.second.begin();

      if ( (DIGITS.find(d) != DIGITS.end()) &&
           (!assign(values, square, d, units, peers)) ) {
        return false; // Fail if we can't assign d to square s.
      }
  }

  if (verbose) {
    std::cout << "Sudoku Output" << std::endl;
    display(values, squares);
  }

  // Report results
  std::stringstream ss;
  for (int i = 0; i < squares.size(); i++) {
    ss << *values[squares[i]].begin();
  }
  grid = ss.str();

  return true;
}

bool solveWithSearch(std::string& grid, bool verbose) {

  // Identify squares
  std::vector<std::string> squares;
  cross(ROWS, COLS, squares);

  // Identify peers and units
  std::unordered_map<std::string,std::unordered_set<std::string>> peers;
  std::unordered_map<std::string,std::vector<
    std::unordered_set<std::string>>> units;

  findPeersUnits(units, peers, squares);

  // To start, every square can be any digit; 
  // then assign values from the grid.
  std::unordered_map<std::string,std::set<char>> values;
  for (const auto& square : squares) {
    values[square] = DIGITS;
  }

  std::unordered_map<std::string,std::set<char>> gvalues;
  gridValues(grid, gvalues, squares);

  if (verbose) {
    std::cout << "Sudoku Input" << std::endl;
    display(gvalues, squares);
  }

  // Constraint Propagation
  for (const auto& cell : gvalues) {
    std::string square = cell.first;
    char d = *cell.second.begin();

    if ( (DIGITS.find(d) != DIGITS.end()) &&
        (!assign(values, square, d, units, peers)) ) {
      return false; // Fail if we can't assign d to square s.
    }
  }

  // Backtracking Search
  bool status = true;
  status = search(status, values, squares, units, peers);

  if (verbose) {
    std::cout << "Sudoku Output" << std::endl;
    display(values, squares);
  }

  // Report results
  std::stringstream ss;
  for (int i = 0; i < squares.size(); i++) {
    ss << *values[squares[i]].begin();
  }
  grid = ss.str();

  return status;
}

} // namespace norvig
} // namespace sudoku
