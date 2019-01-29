cmake_minimum_required(VERSION 3.12)

# define a variable of project name
set(project_name sakura)
set(project_core ${project_name}_core)

# define a project name
project (${project_name} LANGUAGES CXX)

# set the project as the startup project
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${project_name})

# define variables by file GLOB
file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)
file(GLOB_RECURSE RESOURCE_FILES *.rc)

# define sources files of an object library
add_library(${project_core} OBJECT ${SOURCE_FILES} ${HEADER_FILES} ${RESOURCE_FILES})

# define sources files of an executable
add_executable(${project_name} WIN32 $<TARGET_OBJECTS:${project_core}>)

# set include directories
target_include_directories(${project_core}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)

# add preprocessor symbol definitions
#target_compile_definitions(${project_core} PUBLIC _WIN32_WINNT=_WIN32_WINNT_WIN7)

if (MSVC)
  # configure precompiled header
  set(preCompileHeader  "StdAfx.h")
  set(preCompileSource  ${SOURCE_FILES})
  list(FILTER preCompileSource INCLUDE REGEX "StdAfx\\.cpp$" )
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/sakura_core.dir" precompiledBinary ${preCompileSource})
  string(REGEX REPLACE "(\\.cpp)$" "\\1.pch" precompiledBinary "${precompiledBinary}")
  list(REMOVE_ITEM SOURCE_FILES ${preCompileSource})

  # setup source to precompile
  set_source_files_properties(${preCompileSource}
    PROPERTIES
      COMPILE_FLAGS "/Yc${preCompileHeader} /Fp\"${precompiledBinary}\""
	  OBJECT_OUTPUTS "${precompiledBinary}"
  )

  # setup other sources
  set_source_files_properties(${SOURCE_FILES}
    PROPERTIES
      COMPILE_FLAGS "/Yu${preCompileHeader} /Fp\"${precompiledBinary}\""
      OBJECT_DEPENDS "${precompiledBinary}"
  )

  target_compile_options(${project_core} PUBLIC
    $<$<CONFIG:Debug>:           /Od /Ob0 /Zi /RTC1>  # Debug
    $<$<CONFIG:RelWithDebgInfo>: /O2 /Ob1 /Zi /RTC1>  # RelWithDebInfo
    $<$<CONFIG:Release>:         /O2 /Ob2     /RTC1>  # Release
    $<$<CONFIG:MinSizeRel>:      /O1 /Ob1     /RTC1>  # MinSizeRel
  )

  set(compileSources ${SOURCE_FILES})
  list(FILTER compileSources INCLUDE REGEX "\\.cpp$" )
  foreach(cppSource IN LISTS compileSources)
    set(asmFile ${cppSource})
    string(REGEX REPLACE "(\\.cpp)$" "\\1.asm" asmFile ${cppSource})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/sakura_core.dir" asmFile ${asmFile})
    get_source_file_property(compileFlags ${cppSource} COMPILE_FLAGS)
    set_source_files_properties(${cppSource}
      PROPERTIES
        COMPILE_FLAGS "${compileFlags} /FAsu /Fa\"${asmFile}\""
    )
  endforeach()

endif(MSVC)

add_custom_target(githash.h
  "${CMAKE_SOURCE_DIR}/sakura/githash.bat" ../sakura_core
    COMMENT "generate githash.h for c++ code."
    COMMAND_EXPAND_LISTS
  )

add_custom_target(Funccode_enum.h
  HeaderMake -in=${CMAKE_CURRENT_SOURCE_DIR}/Funccode_x.hsrc -out=${CMAKE_CURRENT_BINARY_DIR}/Funccode_enum.h -mode=enum -enum=EFunctionCode > NUL
    DEPENDS HeaderMake
    COMMENT "generate funccode header for c++ code."
    COMMAND_EXPAND_LISTS
    SOURCES Funccode_x.hsrc
  )

add_custom_target(Funccode_define.h
  HeaderMake -in=${CMAKE_CURRENT_SOURCE_DIR}/Funccode_x.hsrc -out=${CMAKE_CURRENT_BINARY_DIR}/Funccode_define.h -mode=define > NUL
    DEPENDS HeaderMake
    COMMENT "generate funccode header for resource script."
    COMMAND_EXPAND_LISTS
    SOURCES Funccode_x.hsrc
  )

add_dependencies(${project_core} githash.h Funccode_enum.h Funccode_define.h)

target_link_libraries(${project_name} comctl32 Imm32 mpr imagehlp Shlwapi winmm windowscodecs msimg32)

if (MINGW)
  set(CMAKE_EXE_LINKER_FLAGS "-static -mwindows -municode")
endif()
