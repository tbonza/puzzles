#include "sudoku.h"
#include <cassert>

namespace sudoku {
namespace norvig {
namespace internal {

void checkCross() {
  std::vector<std::string> squares;
  cross(ROWS, COLS, squares);

  assert(squares.size() == 81);
  assert(squares[0] == "A1");

  std::cout << "  checkCross - PASSED" << std::endl;
}

void checkDisplay() {
  std::string const grid = 
    "4.....8.5.3..........7......2.....6.....8.4....."
    ".1.......6.3.7.5..2.....1.4......";

  std::unordered_map<std::string,std::set<char>> values;
  std::vector<std::string> squares;
  cross(ROWS, COLS, squares);

  gridValues(grid, values, squares);

  display(values, squares);

  std::cout << "  checkDisplay - PASSED" << std::endl;
}

void checkUnitsPeers() {

  std::vector<std::string> squares;
  // Compute indices of squares.
  cross(ROWS, COLS, squares);


  std::unordered_map<std::string, std::unordered_set<std::string>> peers;
  std::unordered_map<std::string, std::vector<
                     std::unordered_set<std::string>>> units;

  findPeersUnits(units, peers, squares);

  assert(squares.size() == 81);

  // Check units for cell 
  std::string cell = "C2";
  std::vector<std::unordered_set<std::string>> c2Units = {
    { "A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2", "I2" },
    { "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9" },
    { "A1", "A2", "A3", "B1", "B2", "B3", "C1", "C2", "C3" },
  };

  for (int i=0; i < 3; i++) {
    for (const auto& u : units[cell][i]) {
      
      if (c2Units[i].find(u) == c2Units[i].end()) {
        std::cout << "units row " << i 
                  << " does not contain " << u << std::endl;
        assert(false);
      }
    }
  }

  // Check peers for cell
  std::vector<std::string> c2Peers = {
    "A2", "B2", "D2", "E2", "F2", "G2", "H2", "I2",
    "C1", "C3", "C4", "C5", "C6", "C7", "C8", "C9",
    "A1", "A3", "B1", "B3" 
  };
  for (const auto& cellPeer : c2Peers) {
    if (peers[cell].find(cellPeer) == peers[cell].end())
      assert(false);
  }

  std::cout << "  checkUnitsPeers - PASSED" << std::endl;
}

void easyEulerSudokuExamples() {
  std::string grid = 
    "003020600"
    "900305001"
    "001806400"
    "008102900"
    "700000008"
    "006708200"
    "002609500"
    "800203009"
    "005010300";

  bool status = solveWithConstraints(grid, false);
  assert(status);

  // From Norvig easy puzzles Project example
  std::string correctGrid = "483921657";  
  for (int i = 0; i < correctGrid.size(); i++)
    assert(correctGrid[i] == grid[i]);

  std::cout << "  easyEulerSudokuExamples - PASSED" << std::endl;
}

void otherNorvigExamples() {
  
  // From Norvig hard puzzles, example 1
  // https://norvig.com/top95.txt

  std::string grid = 
    "4.....8.5"
    ".3......."
    "...7....."
    ".2.....6."
    "....8.4.."
    "....1...."
    "...6.3.7."
    "5..2....."
    "1.4......";

  std::string expect = 
    "417369825"
    "632158947"
    "958724316"
    "825437169"
    "791586432"
    "346912758"
    "289643571"
    "573291684"
    "164875293";

  bool status = solveWithSearch(grid, false);
  assert(status);

  for (int i = 0; i < expect.size(); i++)
    assert(expect[i] == grid[i]);

  std::cout << "  otherNorvigExamples - PASSED" << std::endl;
}

void norvigInkalaExamples() {

  // From Norvig Inkala puzzles, example 1
  // https://usatoday30.usatoday.com/news/offbeat/2006-11-06-sudoku_x.htm
  std::string grid = 
    "85...24.."
    "72......9"
    "..4......"
    "...1.7..2"
    "3.5...9.."
    ".4......."
    "....8..7."
    ".17......"
    "....36.4.";
  std::string expect = 
    "859612437"
    "723854169"
    "164379528"
    "986147352"
    "375268914"
    "241593786"
    "432981675"
    "617425893"
    "598736241";

  bool status = solveWithSearch(grid, false);
  assert(status);

  for (int i = 0; i < expect.size(); i++)
    assert(expect[i] == grid[i]);

  // From Norvig Inkala puzzles, example 2
  // http://www.mirror.co.uk/fun-games/sudoku/2010/08/19/world-s-hardest-sudoku-can-you-solve-dr-arto-inkala-s-puzzle-115875-22496946/
  grid = 
    "..53....."
    "8......2."
    ".7..1.5.."
    "4....53.."
    ".1..7...6"
    "..32...8."
    ".6.5....9"
    "..4....3."
    ".....97..";

  expect = 
    "145327698"
    "839654127"
    "672918543"
    "496185372"
    "218473956"
    "753296481"
    "367542819"
    "984761235"
    "521839764";

  status = solveWithSearch(grid, false);
  assert(status);

  for (int i = 0; i < expect.size(); i++)
    assert(expect[i] == grid[i]);

  std::cout << "  norvigInkalaExamples - PASSED" << std::endl;
}

void tests() {
  checkCross();
  //checkDisplay();
  checkUnitsPeers();
  easyEulerSudokuExamples();
  otherNorvigExamples();
  norvigInkalaExamples();

  std::cout << std::endl;
}

} // namespace internal
} // namespace norvig
} // namespace sudoku
