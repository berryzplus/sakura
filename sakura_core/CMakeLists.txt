cmake_minimum_required(VERSION 3.11)

# define a variable of project name
set(project_name sakura)

# define a project name
project (${project_name})

# set the project as the startup project
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${project_name})

# define variables by file GLOB
file(GLOB_RECURSE SOURCE_FILES   ./*.cpp)
file(GLOB_RECURSE HEADER_FILES   ./*.h)
file(GLOB_RECURSE RESOURCE_FILES ./*.rc)

# define sources files of an executable
add_executable(${project_name} WIN32 ${SOURCE_FILES} ${HEADER_FILES} ${RESOURCE_FILES})

# configure precompiled header
if(MSVC)
	set(PrecompiledSource  "StdAfx.cpp")
	set(PrecompiledHeader  "StdAfx.h")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Yu")
	set_source_files_properties(${PrecompiledSource} PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\"")
endif(MSVC)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

  target_include_directories(${project_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

  add_custom_target(githash.h
	"${CMAKE_CURRENT_SOURCE_DIR}/../sakura/githash.bat" ../sakura_core
      COMMENT generate githash.h for GCC.
      COMMAND_EXPAND_LISTS
    )

  add_custom_target(Funccode_enum.h
    HeaderMake -in=${CMAKE_CURRENT_SOURCE_DIR}/Funccode_x.hsrc -out=../sakura_core/Funccode_enum.h -mode=enum -enum=EFunctionCode
      DEPENDS HeaderMake
      COMMENT generate funccode header for GCC.
      COMMAND_EXPAND_LISTS
      SOURCES Funccode_x.hsrc
    )

  add_custom_target(StdAfx.h.gch
    ${CMAKE_CXX_COMPILER} 
        "-I$<JOIN:$<TARGET_PROPERTY:${project_name},INCLUDE_DIRECTORIES>,;-I>" 
        -D$<JOIN:$<TARGET_PROPERTY:${project_name},COMPILE_DEFINITIONS>,> 
        $<TARGET_PROPERTY:${project_name},COMPILE_OPTIONS> 
        -c ${CMAKE_CURRENT_SOURCE_DIR}/StdAfx.h
        -o ../sakura_core/StdAfx.h.gch
      DEPENDS Funccode_enum.h
      COMMENT generate pre-compiled header for GCC.
      COMMAND_EXPAND_LISTS
      SOURCES StdAfx.h
    )

  add_custom_target(Funccode_define.h
    HeaderMake -in=${CMAKE_CURRENT_SOURCE_DIR}/Funccode_x.hsrc -out=../sakura_core/Funccode_define.h -mode=define
      DEPENDS HeaderMake
      COMMENT generate funccode header for windres resource.
      COMMAND_EXPAND_LISTS
      SOURCES Funccode_x.hsrc
    )

  add_dependencies(${project_name} githash.h StdAfx.h.gch Funccode_define.h)

  target_compile_options(${project_name}
    PRIVATE -finput-charset=utf-8 -fexec-charset=cp932
    )

  set_target_properties(${project_name}
    PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++ -mwindows"
    )

endif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

# add definitions
target_compile_definitions(${project_name} PUBLIC _WINDOWS UNICODE _UNICODE _WIN32_WINNT=0x0500 NOMINMAX)

# add include directories
target_include_directories(${project_name} PRIVATE ./)

# add library modules
target_link_libraries(${project_name} comctl32 winspool ole32 oleaut32 uuid mpr imagehlp shlwapi imm32 winmm)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set_target_properties(${project_name}
    PROPERTIES LINK_FLAGS "-mwindows")
  set(CMAKE_RC_FLAGS "-c utf8")
endif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
