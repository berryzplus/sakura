cmake_minimum_required(VERSION 3.11)

# define a variable of project name
set(project_name sakura_lang_en_US)

# define a project name
project (${project_name})

# define variables by file GLOB
file(GLOB_RECURSE SOURCE_FILES   *.cpp)
file(GLOB_RECURSE HEADER_FILES   *.h)
file(GLOB_RECURSE RESOURCE_FILES *.rc)

# define project as a DLL
add_library(${project_name} MODULE ${SOURCE_FILES} ${HEADER_FILES} ${RESOURCE_FILES})

# add definitions
target_compile_definitions(${project_name} PUBLIC _WINDOWS UNICODE _UNICODE)

# add include directories
target_include_directories(${project_name} PRIVATE ./)

# avoid error LNK2001 for "__DllMainCRTStartup@12"
set_target_properties(${project_name} PROPERTIES LINK_FLAGS "/NOENTRY")


if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

  target_include_directories(${project_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../sakura_core)
  add_dependencies(${project_name} githash.h Funccode_define.h)

  set_target_properties(${project_name}
    PROPERTIES
      LIBRARY_OUTPUT_NAME ${project_name}
    )

endif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
