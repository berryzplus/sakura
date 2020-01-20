cmake_minimum_required(VERSION 3.12)

project (sakura-unittest LANGUAGES CXX)

# find GoogleTest
find_package(GTest)

if (NOT GTest_FOUND)

  # fetch the GoogleTest source.
  if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt)
    find_package(Git REQUIRED)
    execute_process(COMMAND "${GIT_EXECUTABLE}" submodule update --init
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/googletest
  	)
  endif()
  
  # make directory for the GoogleTest to build
  if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/googletestd)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletestd)
  endif()
  # make directory for the GoogleTest to build
  if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/googletest)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest)
  endif()
  
  # make GoogleTest: configure, build and install.
  function(install_gtest gtest_install_dir gtest_config gtest_binary_dir)
    execute_process(
      COMMAND "${CMAKE_COMMAND}" 
  	  -G "${CMAKE_GENERATOR}"
        -DCMAKE_BUILD_TYPE=${gtest_config}
        "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
        "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
        "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
        -DCMAKE_INSTALL_PREFIX=${gtest_install_dir}
        -DBUILD_GMOCK=OFF
        -Dgtest_build_tests=OFF
        -Dgtest_build_samples=OFF
        ${CMAKE_CURRENT_SOURCE_DIR}/googletest
      WORKING_DIRECTORY ${gtest_binary_dir}
      )
    execute_process(
      COMMAND "${CMAKE_COMMAND}"
  	  --build .
  	  --config ${gtest_config}
      WORKING_DIRECTORY ${gtest_binary_dir}
      )
    execute_process(
      COMMAND "${CMAKE_COMMAND}"
  	  --build .
  	  --config ${gtest_config}
  	  --target install
      WORKING_DIRECTORY ${gtest_binary_dir}
      )
  endfunction(install_gtest)
  
  install_gtest(${CMAKE_INSTALL_PREFIX}/googletest Debug   ${CMAKE_CURRENT_BINARY_DIR}/googletestd)
  install_gtest(${CMAKE_INSTALL_PREFIX}/googletest Release ${CMAKE_CURRENT_BINARY_DIR}/googletest)
  
  set(GTEST_ROOT ${CMAKE_INSTALL_PREFIX}/googletest)
  find_package(GTest REQUIRED)

endif (NOT GTest_FOUND)

# set Target Directories
add_subdirectory(unittests)
