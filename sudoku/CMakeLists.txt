# Copyright 2022 Puz Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license 
# that can be found in the LICENSE file. See the AUTHORS file for 
# names of contributors.

cmake_minimum_required(VERSION 3.18)

project(
  puz
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(PUZ_BUILD_TESTS "Build Puz's unit tests" ON)
option(PUZ_BUILD_BENCHMARKS "Build Puz's benchmarks" ON)

if(PUZ_BUILD_TESTS)
  # Enable testing in this directory so we can do a top-level `make test`. 
  # This also includes the BUILD_TESTING option, which is on by default.
  include(CTest)

  enable_testing()

endif(PUZ_BUILD_TESTS)

add_subdirectory(libpuz)

if(PUZ_BUILD_BENCHMARKS)

  find_package(benchmark REQUIRED)
  add_subdirectory(puz_benchmark)

endif(PUZ_BUILD_BENCHMARKS)
