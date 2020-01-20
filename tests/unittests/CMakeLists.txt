cmake_minimum_required(VERSION 3.12)

# define a variable of project name
set(project_name tests1)

# define variables by file
file(GLOB TEST_FILES test-*.cpp)

# define sources files of an executable
add_executable(${project_name} ${TEST_FILES} $<TARGET_OBJECTS:sakura_core>)

# add preprocessor symbol definitions
target_compile_definitions(${project_name} PUBLIC _CONSOLE)

# add link libraries
target_link_libraries(${project_name}
  PRIVATE sakura_core
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  )

# add test
add_test(NAME UnitTests COMMAND ${project_name} -v)
