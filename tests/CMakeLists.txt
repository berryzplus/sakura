cmake_minimum_required(VERSION 3.12)
#include(${CMAKE_SOURCE_DIR}/runtime.cmake)

project (sakura-unittest LANGUAGES CXX)

# enable testing
enable_testing()

# set Target Directories
add_subdirectory(unittests)

# add test
add_test(NAME unitTest1 COMMAND tests1)
