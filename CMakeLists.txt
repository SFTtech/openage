# Copyright 2013-2017 the openage authors. See copying.md for legal info.

cmake_minimum_required(VERSION 3.1.0)
# required for CMAKE_CXX_STANDARD

# main build configuration file

# text art: figlet -f rounded "[SFT] openage" | sed -e 's/\\/\\\\/g'
message("

 ___  ______ _______ _______ ___
|  _)/ _____|_______|_______|_  |
| | ( (____  _____      _     | |    ___  ____  _____ ____  _____  ____ _____
| |  \\____ \\|  ___)    | |    | |   / _ \\|  _ \\| ___ |  _ \\(____ |/ _  | ___ |
| |_ _____) ) |        | |   _| |  | |_| | |_| | ____| | | / ___ ( (_| | ____|
|___|______/|_|        |_|  (___|   \\___/|  __/|_____)_| |_\\_____|\\___ |_____)
                                         |_|                     (_____|

Welcome to the SFT technologies computer-aided openage build system!

You have chosen, or been chosen, to attempt the daring task of building openage.
If you have installed all the dependencies that are conveniently listed in
[doc/building.md], this _might_ just work!

If it doesn't, consider reporting the issue: https://github.com/SFTtech/openage
Or ask for help:
  * Matrix: #sfttech:matrix.org
  * IRC:    #sfttech on freenode.net
")

# cmake < 3.4.0 has problems with projects that use C++ but not C,
# especially in the FindThreads module (when header files are checked).
if(CMAKE_VERSION VERSION_LESS "3.4.0")
	message(WARNING "using cmake ${CMAKE_VERSION} < 3.4.0, falling back and setting CC=CXX.")
	project(openage C CXX)

	# simple hack to set CC=CXX because
	# older cmake requires CC for some header tests
	set(CMAKE_C_COMPILER "${CMAKE_CXX_COMPILER}")
	set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
else()
	project(openage CXX)
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure CMAKE_BUILD_TYPE is set correctly.
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" BUILD_TYPE_CXX_FLAGS)

# options: keep up to date with those in ./configure!
if(NOT DEFINED WANT_BACKTRACE)
	set(WANT_BACKTRACE if_available)
endif()

if(NOT DEFINED WANT_INOTIFY)
	set(WANT_INOTIFY if_available)
endif()

if(NOT DEFINED WANT_GPERFTOOLS_PROFILER)
	set(WANT_GPERFTOOLS_PROFILER if_available)
endif()

if(NOT DEFINED WANT_GPERFTOOLS_TCMALLOC)
	set(WANT_GPERFTOOLS_TCMALLOC false)
endif()

# static content filesystem locations
if(NOT DEFINED GLOBAL_ASSET_DIR)
	set(ASSET_DIR "share/openage")
	if(MSVC)
		set(GLOBAL_ASSET_DIR "${ASSET_DIR}")
	else()
		set(GLOBAL_ASSET_DIR "${CMAKE_INSTALL_PREFIX}/${ASSET_DIR}")
	endif()
endif()

if(NOT DEFINED GLOBAL_CONFIG_DIR)
	set(CONFIG_DIR "etc/openage")
	if(MSVC)
		set(GLOBAL_CONFIG_DIR "${CONFIG_DIR}")
	else()
		set(GLOBAL_CONFIG_DIR "/${CONFIG_DIR}")
	endif()
endif()


# build the project with ccache
# distros can also do this but they don't use this mechanism
option(ENABLE_CCACHE "prefix each compile command with ccache")

if(ENABLE_CCACHE)
	find_program(CCACHE_FOUND "ccache")

	if(CCACHE_FOUND)
		set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
		set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
	else()
		message(FATAL_ERROR "ccache not found, but you requested it")
	endif(CCACHE_FOUND)
endif()


# option processing is now done.


# add search paths to helper modules
set(BUILDSYSTEM_DIR "${CMAKE_SOURCE_DIR}/buildsystem")
set(CMAKE_MODULE_PATH "${BUILDSYSTEM_DIR}" "${BUILDSYSTEM_DIR}/modules/")

# load helper modules
include(GNUInstallDirs)
include(CheckInSourceBuild)
include(HandleCXXOptions)
include(CheckCompilerFeatures)
include(CMakeParseArguments)
include(HandlePythonOptions)
include(CheckRuntimeDependencies)
include(DetectProjectVersion)
include(DependencyFetch)

# include build configuration modules
include(CTest)

# initialize language support
include(codegen)
include(cpp)
include(doxygen)
include(options)
include(python)
include(util)


# now that all modules and settings are loaded,
# apply those to the project:

# create documentation
doxygen_configure(libopenage/ openage/ doc/ README.md)

# static content
add_subdirectory(assets/)
add_subdirectory(dist/)
add_subdirectory(cfg/)

# C++ content
add_subdirectory(libopenage/)

# Python content (uses the C++ library)
set(PYEXT_LINK_LIBRARY libopenage)
if(MSVC)
	set(FORCE_INCLUDE_CXXFLAG "/FI")
else()
	set(FORCE_INCLUDE_CXXFLAG "-include ")
endif()
set(PYEXT_CXXFLAGS "${PYEXT_CXXFLAGS} ${FORCE_INCLUDE_CXXFLAG}\"${CMAKE_SOURCE_DIR}/libopenage/pyinterface/hacks.h\"")
add_cython_modules(EMBED NOINSTALL run.py)
add_py_modules(BININSTALL run.py AS openage)
add_subdirectory(openage/)

python_finalize()

# Ensure that packaging is always the last step.
add_subdirectory(packaging)

message("")

print_config_options()

# show build configuration overview
message("${PROJECT_NAME} ${PROJECT_VERSION}

         compiler | ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}
       build type | ${CMAKE_BUILD_TYPE}
         cxxflags | ${CMAKE_CXX_FLAGS}
 build type flags | ${${BUILD_TYPE_CXX_FLAGS}}
        build dir | ${CMAKE_BINARY_DIR}
   install prefix | ${CMAKE_INSTALL_PREFIX}
py install prefix | ${CMAKE_PY_INSTALL_PREFIX}
")

# don't print 'Built target ...' messages
# upstream since cmake v3.4.0-rc1 (by commit 1d3984780df8)
set_property(GLOBAL PROPERTY TARGET_MESSAGES OFF)
