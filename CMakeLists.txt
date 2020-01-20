cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)          # C++17...
set(CMAKE_CXX_STANDARD_REQUIRED ON) # C++17 is required...
set(CMAKE_CXX_EXTENSIONS OFF)		# -std=c++17

project (SakuraEditor LANGUAGES CXX)

# add generic preprocessor symbol definitions
add_compile_definitions(
  $<IF:$<CONFIG:Debug>,_DEBUG,NDEBUG>
  _UNICODE
  UNICODE
   _WIN32_WINNT=_WIN32_WINNT_WIN7
)

if (MINGW)
  # add generic compile options for GCC
  add_compile_options(
    -finput-charset=utf-8
    -fexec-charset=cp932
  )
  set(CMAKE_RC_FLAGS "-c utf8 --language=0411")
  set(CMAKE_EXE_LINKER_FLAGS "-static")
endif (MINGW)

add_subdirectory(HeaderMake)
add_subdirectory(sakura_core)
add_subdirectory(sakura_lang_en_US)

enable_testing()
add_subdirectory(tests)
