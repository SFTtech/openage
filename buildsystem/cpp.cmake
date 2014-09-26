# sets CXXFLAGS and compiler for the project

#TODO: integrate PGO (profile-guided optimization) build

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}")

message("compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

macro(cxx_required CXXNAME MINIMAL)
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MINIMAL})
		message(FATAL_ERROR ">=${CXXNAME}-${MINIMAL} required (c++11, you know?), you have ${CMAKE_CXX_COMPILER_VERSION}")
	endif()
endmacro()

# check for compiler versions
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	cxx_required("gcc" 4.8)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	cxx_required("clang" 3.3)
else() #"Intel", "MSVC", etc..
	message(WARNING "Using untested compiler, at least I hope it's free software. Continue on your own, warrior.")
endif()

# these environment variables are used by the python C extension builder
set(CCENV "CC=${CMAKE_C_COMPILER}")
set(CXXENV "CXX=${CMAKE_CXX_COMPILER}")
