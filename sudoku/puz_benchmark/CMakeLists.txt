# Copyright 2022 Puz Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license 
# that can be found in the LICENSE file. See the AUTHORS file for 
# names of contributors.

if(PUZ_BUILD_BENCHMARKS)

  find_package(benchmark REQUIRED)

  set(PUZ_BENCHMARK_FILES
    sudoku_norvig.cc)

  add_executable(puz_benchmark ${PUZ_BENCHMARK_FILES})
  target_link_libraries(puz_benchmark PRIVATE
            puz-lib
            benchmark::benchmark)

  set (source "${CMAKE_CURRENT_SOURCE_DIR}/resources")
  set (destination "${CMAKE_CURRENT_BINARY_DIR}/resources")
  add_custom_command(
    TARGET ${PROJECT_NAME}_benchmark POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
    DEPENDS ${destination}
    COMMENT "symbolic link resources folder from ${source} => ${destination}"
  )

endif(PUZ_BUILD_BENCHMARKS)
