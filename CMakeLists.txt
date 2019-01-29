cmake_minimum_required(VERSION 3.12)

project (SakuraEditor LANGUAGES CXX)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# add generic preprocessor symbol definitions
add_compile_definitions(
  _UNICODE
  UNICODE
   _WIN32_WINNT=_WIN32_WINNT_WIN7
)

set(buildType)
string(TOUPPER ${CMAKE_BUILD_TYPE} buildType)

if (buildType STREQUAL DEBUG)
  add_compile_definitions( _DEBUG )
else ()
  add_compile_definitions( NDEBUG )
endif ()

if (MSVC)
  # split CXXFLAGS properly.
  set(compileOptions)
  string(REGEX REPLACE "\\s{2,}" " " compileOptions ${CMAKE_CXX_FLAGS})
  separate_arguments(compileOptions WINDOWS_COMMAND ${compileOptions})

  set(compileDefinitions ${compileOptions})
  list(FILTER compileDefinitions INCLUDE REGEX "/D.+" )
  list(TRANSFORM compileDefinitions REPLACE "/D" "" )
  add_compile_definitions(${compileDefinitions})

  list(FILTER compileOptions EXCLUDE REGEX "/D.+" )
  add_compile_options(${compileOptions})
  list(JOIN compileOptions " " compileOptions)
  set(CMAKE_CXX_FLAGS "")

  string(REGEX REPLACE "\\s{2,}" " " compileOptions ${CMAKE_CXX_FLAGS_${buildType}})
  separate_arguments(compileOptions WINDOWS_COMMAND ${compileOptions})

  set(compileDefinitions ${compileOptions})
  list(FILTER compileDefinitions INCLUDE REGEX "/D.+" )
  list(TRANSFORM compileDefinitions REPLACE "/D" "" )
  add_compile_definitions(${compileDefinitions})

  list(FILTER compileOptions EXCLUDE REGEX "/D.+" )
  list(TRANSFORM compileOptions REPLACE "/MD" "/MT" )
  add_compile_options(${compileOptions})
  list(JOIN compileOptions " " compileOptions)
  set(CMAKE_CXX_FLAGS_${buildType} "")

  # split RCFLAGS properly.
  set(compileOptions)
  string(REGEX REPLACE "\\s{2,}" " " compileOptions ${CMAKE_RC_FLAGS})
  separate_arguments(compileOptions WINDOWS_COMMAND ${compileOptions})

  set(compileDefinitions ${compileOptions})
  list(FILTER compileDefinitions INCLUDE REGEX "/D.+" )
  list(TRANSFORM compileDefinitions REPLACE "/D" "" )
  add_compile_definitions(${compileDefinitions})

  list(FILTER compileOptions EXCLUDE REGEX "/D.+" )
  add_compile_options(${compileOptions})
  list(JOIN compileOptions " " compileOptions)
  set(CMAKE_RC_FLAGS "/nologo")

  string(REGEX REPLACE "\\s{2,}" " " compileOptions ${CMAKE_RC_FLAGS_${buildType}})
  separate_arguments(compileOptions WINDOWS_COMMAND ${compileOptions})

  set(compileDefinitions ${compileOptions})
  list(FILTER compileDefinitions INCLUDE REGEX "/D.+" )
  list(TRANSFORM compileDefinitions REPLACE "/D" "" )
  add_compile_definitions(${compileDefinitions})

  list(FILTER compileOptions EXCLUDE REGEX "/D.+" )
  add_compile_options(${compileOptions})
  list(JOIN compileOptions " " compileOptions)
  set(CMAKE_RC_FLAGS_${buildType} "")

  # add generic compile options for MSVC
  add_compile_options(
    /MP 
    /GS 
    /Gm- 
    /Gd 
    /Zc:forScope 
    /Zc:inline 
    /Zc:wchar_t 
    /diagnostics:classic 
    /fp:precise 
    /source-charset:utf-8 
    /execution-charset:shift_jis 
    /errorReport:prompt 
  )

endif (MSVC)

if (MinGW)
  # add generic compile options for MSVC
  add_compile_options(
    -finput-charset=utf-8
    -fexec-charset=cp932
  )

endif (MinGW)

add_subdirectory(HeaderMake)
add_subdirectory(sakura_core)
add_subdirectory(sakura_lang_en_US)
add_subdirectory(tests)
