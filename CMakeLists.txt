cmake_minimum_required(VERSION 2.8.7)
#v2.8.6-306-gfa7141f introduced compiler version detection

project(openage)

#main build configuration file
#could use some tuning, but we are too lazy..

#text art: figlet -f rounded "[SFT] openage" | sed -e 's/\\/\\\\/g'
message("==============================================================================")
message(" ___  ______ _______ _______ ___                                              ")
message("|  _)/ _____|_______|_______|_  |                                             ")
message("| | ( (____  _____      _     | |    ___  ____  _____ ____  _____  ____ _____ ")
message("| |  \\____ \\|  ___)    | |    | |   / _ \\|  _ \\| ___ |  _ \\(____ |/ _  | ___ |")
message("| |_ _____) ) |        | |   _| |  | |_| | |_| | ____| | | / ___ ( (_| | ____|")
message("|___|______/|_|        |_|  (___|   \\___/|  __/|_____)_| |_\\_____|\\___ |_____)")
message("                                         |_|                     (_____|      ")
message("")
message("Welcome to the SFT Technologies computer aided openage build system!")
message("  We are glad you decided to use our product.")
message("  To make your experience with the software more fun, we offer a wide range of cutting edge public relation campaigns:")
message("  - Would you like to sell your personal data? Become a ultra-special premium user with no additional privileges!")
message("  - You can start gaining fame with our unique loyalty point collecting service!")
message("  - We automatically subscribed you to our daily Email newsletter, how awesome is that?")
message("")
message("  We hope your consumery needs will be fully fullfilled,")
message("  otherwise, utilize our professional customer support team:")
message("  *  XMPP MUC: openage@chat.sft.mx")
message("  *  IRC:      #sfttech  at freenode.org")
message("")
message("==============================================================================")

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
