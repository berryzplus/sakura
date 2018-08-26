cmake_minimum_required(VERSION 3.11)

project (SakuraEditor)

if (CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "DEBUG")
endif(CMAKE_BUILD_TYPE STREQUAL "")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -D_DEBUG")
  set(CMAKE_RC_FLAGS "-c utf8 --language=0411")
endif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")


include(GNUInstallDirs)

add_subdirectory(HeaderMake)
add_subdirectory(sakura_core)

# DLLのビルドがうまくできないので保留
#add_subdirectory(sakura_lang_en_US)
