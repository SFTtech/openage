cmake_minimum_required(VERSION 2.8.10)
#git tag --contains=fa7141f => 2.8.8    # compiler version detection
#git tag --contains=fbda7bb => 2.8.10   # find GLEW module

project(openage C CXX)

# main build configuration file
# could use some tuning, but we are too lazy..

# text art: figlet -f rounded "[SFT] openage" | sed -e 's/\\/\\\\/g'
message("")
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
message("==============================================================================")
message("")

#TODO: integrate PGO build

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/lib")

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

# include these directories in the build process
add_subdirectory("src")
add_subdirectory("convert")

# Doxygen integration
find_package(Doxygen)
if(DOXYGEN_FOUND)
	find_file(DOT dot HINTS /usr/bin/dot)

	set(DOT_EXECUTABLE "/bin/false")
	set(HAVE_DOT "NO")

	if(NOT ${DOT} STREQUAL "DOT-NOTFOUND")
		set(DOT_EXECUTABLE "${DOT}")
		set(HAVE_DOT "YES")
		message(STATUS "Found dot: ${DOT_EXECUTABLE}")
	else()
		message(WARNING "graphviz dot couldn't be found, you won't have cool graphs in the docs.")
	endif()

	# adapt doxygen config
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

	# add doc target
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "generating docs with Doxygen" VERBATIM
	)
endif()

message("CFLAGS   = ${CMAKE_C_FLAGS}")
message("CXXFLAGS = ${CMAKE_CXX_FLAGS}")
message("LDFLAGS  = ${CMAKE_EXE_LINKER_FLAGS}")
