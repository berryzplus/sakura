cmake_minimum_required(VERSION 3.12)

# define a variable of project name
set(project_name sakura_lang_en_US)

# define a project name
project (${project_name} LANGUAGES CXX)

# define variables by file GLOB
file(GLOB_RECURSE SOURCE_FILES *.cpp *.h *.rc)

# define sources files of an executable
add_library(${project_name} SHARED ${SOURCE_FILES})

# add include directories for sakura_core
target_include_directories(${project_name}
  PRIVATE ${CMAKE_SOURCE_DIR}/sakura_core
  PRIVATE ${CMAKE_BINARY_DIR}/sakura_core
)

target_compile_options(${project_name} PUBLIC /nologo)

# fix the error 'Missing variable is: CMAKE_RC_CREATE_SHARED_LIBRARY'
# https://cmake.org/pipermail/cmake/2012-January/048647.html
set_target_properties(${project_name} PROPERTIES LINKER_LANGUAGE "CXX" )

if(MSVC)
	# avoid error LNK2001 for "__DllMainCRTStartup@12"
	set_target_properties(${project_name}
      PROPERTIES
        LINK_FLAGS	"/NOENTRY"
    )
elseif(MINGW)
    # avoid prefixing of DLL name, set PREFIX to blank.
	# https://cmake.org/cmake/help/v3.12/prop_tgt/PREFIX.html?highlight=prefix
	set_target_properties(${project_name}
	  PROPERTIES
	    PREFIX		""
    )
endif()

add_dependencies(${project_name} githash.h Funccode_define.h)
