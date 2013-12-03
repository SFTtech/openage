cmake_minimum_required(VERSION 2.8.7)
#v2.8.6-306-gfa7141f introduced compiler version detection

project(openage)

#main build configuration file

#maybe use clang
#set(CMAKE_CXX_COMPILER "clang++" )

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++11 -march=native -mtune=native")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/lib")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Og")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Werror -O3 -flto=4")

#check for compiler versions
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
		message(FATAL_ERROR ">=gcc-4.8 required (c++11, you know?)")
	endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3)
		message(FATAL_ERROR ">=clang-3.3 required (c++11, you know?)")
	endif()
else() #"Intel", "MSVC", etc..
	message(WARNING "Using untested compiler, at least I hope it's free software. Continue on your own, warrior.")
endif()

#add our source directory, it contains the cmake config with the file list
add_subdirectory("src")

# Doxygen integration
find_package(Doxygen)
if(DOXYGEN_FOUND)
	#this adds the correct folder to INPUT= :
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

	#add doc target
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "generating docs with Doxygen" VERBATIM
	)
endif(DOXYGEN_FOUND)
