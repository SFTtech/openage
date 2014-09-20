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

set(CCENV "CC=${CMAKE_C_COMPILER}")
set(CXXENV "CXX=${CMAKE_CXX_COMPILER}")


#look for the python3.3 executable
find_program(PYTHON3 python3 HINTS /usr/bin/python3.3 /usr/bin/python3.4)

if(${PYTHON3} STREQUAL "PYTHON3-NOTFOUND")
	message(FATAL "python >= 3.3 not found.")
else()
	message(STATUS "Found python executable: ${PYTHON3}")
endif()


# list of python package names to export via distutils.
set(PY_MODULES)
set(PYEXT_MODULES)

# helper function to add packages to this list.
macro (add_py_module name)
	list(APPEND PY_MODULES ${name})
	set(PY_MODULES ${PY_MODULES} PARENT_SCOPE)
endmacro()

# add a python c extension module, with given sources
set(PYEXT_ARRAY "PYEXT_MODULE_")
macro(add_pyext_module)
	set(options "LOL___") # unused
	set(one_value_args "NAME")
	set(multi_value_args "SOURCES")
	cmake_parse_arguments(apyext "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

	message("================= name=${apyext_NAME}")

	if(DEFINED "${PYEXT_ARRAY}${apyext_NAME}")
		message(ERROR "The Python extension module ${apyext_NAME} was already defined!")
	endif()

	set(pyext_sources)
	file(RELATIVE_PATH current_path "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	foreach(srcfile ${apyext_SOURCES})
		if(current_path)
			list(APPEND pyext_sources "${current_path}/${srcfile}")
		else()
			list(APPEND pyext_sources "${srcfile}")
		endif()
	endforeach()

	message("=== Creating new python module '${apyext_NAME}'. sources:\n ${pyext_sources}")
	list(APPEND PYEXT_MODULES "${apyext_NAME}")
	set(PYEXT_MODULES "${PYEXT_MODULES}" PARENT_SCOPE)

	# add a new "dict" key. fak u cmake.
	set("${PYEXT_ARRAY}${apyext_NAME}" "${pyext_sources}" PARENT_SCOPE)
endmacro()

################################################################################
# include subdirectories in the build process

add_subdirectory("src")
add_subdirectory("convert")

################################################################################



# generate content for the python distutils setup.py script
set(PYEXT_SOURCEDEF)
set(PY_SOURCES_ALL)

# iterate over all C extension modules and create python dict entries.
# module_name => [module_source_file,...]
foreach(pyext_module ${PYEXT_MODULES})
	set(PYEXT_KEYNAME "${PYEXT_ARRAY}${pyext_module}")
	set(PYEXT_SOURCEDEF "${PYEXT_SOURCEDEF}\n    '${pyext_module}': '${${PYEXT_KEYNAME}}'.split(';'),")

	foreach(pyext_source "${${PYEXT_KEYNAME}}")
		list(APPEND PY_SOURCES_ALL "${pyext_source}")
	endforeach()
endforeach()

# TODO: populate PY_SOURCES_ALL with all .py files found
#       in the PY_MODULES directories!
#       this will trigger nice distutil rebuilds


set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
set(PY_TIMEFILE "${CMAKE_CURRENT_BINARY_DIR}/build/py_mods")

# create setup.py file for python module creation.
#
# * the time stamp file is used for rebuilding
# * all python-C extensions are built inplace
#   so they can be used for imports in the development tree
# * the pure python modules are compiled to pyc
configure_file(${SETUP_PY_IN} ${SETUP_PY})
add_custom_command(OUTPUT ${PY_TIMEFILE}
                   COMMAND ${CXXENV} ${CCENV} ${PYTHON3} ${SETUP_PY} build_ext --inplace
                   COMMAND ${CXXENV} ${CCENV} ${PYTHON3} ${SETUP_PY} build
                   COMMAND ${CMAKE_COMMAND} -E touch ${PY_TIMEFILE}
                   DEPENDS ${PY_SOURCES_ALL})
add_custom_target(python_modules ALL DEPENDS ${PY_TIMEFILE})

# create call to setup.py when installing.
# * set the install prefix at configure time
# * evaluate the temporary install destination parameter DESTDIR at 'make install'-time
install(CODE "execute_process(COMMAND ${PYTHON3} ${SETUP_PY} install --prefix=${CMAKE_INSTALL_PREFIX} --root=\$ENV{DESTDIR})")

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

configure_file(${CMAKE_SOURCE_DIR}/Makefile.in ${CMAKE_SOURCE_DIR}/Makefile @ONLY)

message("CFLAGS   = ${CMAKE_C_FLAGS}")
message("CXXFLAGS = ${CMAKE_CXX_FLAGS}")
message("LDFLAGS  = ${CMAKE_EXE_LINKER_FLAGS}")
message("PREFIX   = ${CMAKE_INSTALL_PREFIX}")
