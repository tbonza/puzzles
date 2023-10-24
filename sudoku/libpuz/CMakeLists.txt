# Copyright 2022 Puz Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license 
# that can be found in the LICENSE file. See the AUTHORS file for 
# names of contributors.

add_library(
  puz-lib #cmake-format: sort
  sudoku.h
  sudoku_solver.cc)

target_include_directories(
    puz-lib PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
                   $<INSTALL_INTERFACE:include>)

if(PUZ_BUILD_TESTS)

  set(PUZ_TEST_FILES
    test_runner.h
    test_runner.cc
    sudoku_solver_test.cc)

  add_executable(puz_test ${PUZ_TEST_FILES})
  target_link_libraries(puz_test PRIVATE
    puz-lib)

endif(PUZ_BUILD_TESTS)



