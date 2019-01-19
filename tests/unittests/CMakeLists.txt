cmake_minimum_required(VERSION 3.12)

# define a variable of project name
set(project_name tests1)

# define variables by file
file(GLOB TEST_FILES *.cpp)

# define sources files of an executable
add_executable(${project_name} ${TEST_FILES} $<TARGET_OBJECTS:sakura_core>)
add_dependencies(${project_name} sakura_core sakura)

# add include directories for sakura_core
target_include_directories(${project_name}
  PRIVATE ${CMAKE_SOURCE_DIR}/sakura_core
  PRIVATE ${CMAKE_BINARY_DIR}/sakura_core
)

# add preprocessor symbol definitions
target_compile_definitions(${project_name} PRIVATE _CONSOLE)

# google test
if(MSVC)
  # restore nuget packages into current source directory
  execute_process(
    COMMAND
	  nuget restore packages.config -PackagesDirectory packages
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
  set(GTEST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/packages/Microsoft.googletest.v140.windesktop.msvcstl.static.rt-static.1.8.0)
  set(GTEST_MSVC_SEARCH MT)
  include(FindNugetGTest.cmake)

  # add preprocessor symbol definitions for gtest
  target_compile_definitions(${project_name} PRIVATE _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)

elseif(MINGW)
  find_package(GTest REQUIRED)
endif()

# link libraries
target_link_libraries(${project_name} PRIVATE GTest::GTest GTest::Main)

target_link_libraries (${project_name} PRIVATE comctl32 imm32 mpr imagehlp shlwapi winmm windowscodecs msimg32)
