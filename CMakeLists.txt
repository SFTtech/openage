# Copyright 2013-2020 the openage authors. See copying.md for legal info.

cmake_minimum_required(VERSION 3.12)

# required for FindPython3.cmake
# >3.8 required for CMAKE_CXX_STANDARD for C++17

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


##################################################
# main buildsystem setup entry point
project(openage CXX)

# C++ standard requirement
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Python and Cython requirements
set(PYTHON_MIN_VERSION 3.6)
set(CYTHON_MIN_VERSION 0.25)

# CMake policies
foreach(pol
        CMP0067  # honor language standard in try_compile()
        CMP0071  # enable automoc for generated files
        CMP0072  # prefers GLVND by default FindOpenGL
        CMP0048  # project() command manages VERSION variables
        CMP0094  # take the first satisfying Python version
       )
	if (POLICY ${pol})
		cmake_policy(SET ${pol} NEW)
	endif()
endforeach()

# don't print 'Built target ...' messages
# upstream since cmake v3.4.0-rc1 (by commit 1d3984780df8)
set_property(GLOBAL PROPERTY TARGET_MESSAGES OFF)

# Ensure CMAKE_BUILD_TYPE is set correctly.
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" BUILD_TYPE_CXX_FLAGS)


##################################################
# options: keep up to date with those in ./configure!
if(NOT DEFINED WANT_BACKTRACE)
	set(WANT_BACKTRACE if_available)
endif()

if(NOT DEFINED WANT_INOTIFY)
	set(WANT_INOTIFY if_available)
endif()

if(NOT DEFINED WANT_OPENGL)
	set(WANT_OPENGL if_available)
endif()

if(NOT DEFINED WANT_VULKAN)
	set(WANT_VULKAN if_available)
endif()

if(NOT DEFINED WANT_GPERFTOOLS_PROFILER)
	set(WANT_GPERFTOOLS_PROFILER if_available)
endif()

if(NOT DEFINED WANT_GPERFTOOLS_TCMALLOC)
	set(WANT_GPERFTOOLS_TCMALLOC false)
endif()

if(NOT DEFINED WANT_NCURSES)
	set(WANT_NCURSES if_available)
endif()

##################################################
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


##################################################
# ccache setup

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


##################################################
# clang tidy static analysis
option(
	ENABLE_CLANG_TIDY
	"activate clang tidy messages"
	OFF
)
if(ENABLE_CLANG_TIDY)
	# .clang-tidy in repository is used
	set(CMAKE_CXX_CLANG_TIDY "clang-tidy;--export-fixes=${CMAKE_CURRENT_BINARY_DIR}/clang-tidy-fixes.yaml, \
	--extra-arg=-std=c++17")
endif()


# option processing is now done.


##################################################
# include buildsystem features

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
include(FindPkgConfig)
include(FindPackageHandleStandardArgs)

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
# apply those to the project.


##################################################
# set project version
if(USED_GIT_VERSION)
	# VERSION_FULL_STRING is the full git describe
	set(VERSION_FULL_STRING "${PROJECT_VERSION}")
	# PROJECT_VERSION is MAJOR.MINOR.PATCH.TWEAK with Commit-Count as Tweak
	STRING(REGEX REPLACE "v(([0-9]+.|[0-9]+)+)-([0-9]+)-g([a-f0-9]+)"
			"\\1.\\3" PROJECT_VERSION "${VERSION_FULL_STRING}")
endif()
project(openage VERSION "${PROJECT_VERSION}")

# set CI version
if(DEFINED ENV{CI_CFG_VERSION})
    set(CI_CFG_VERSION "$ENV{CI_CFG_VERSION}")
else()
    set(CI_CFG_VERSION "NOT SET")
endif()


##################################################
# documentation generation

# create documentation
doxygen_configure(libopenage/ openage/ doc/ README.md)


##################################################
# static content
add_subdirectory(assets/)
add_subdirectory(dist/)
add_subdirectory(cfg/)


##################################################
# C++ content
add_subdirectory(libopenage/)


##################################################
# Python content (uses the C++ library)
add_library(pyext_libopenage INTERFACE)
if(MSVC)
	set(FORCE_INCLUDE_CXXFLAG "/FI")
else()
	set(FORCE_INCLUDE_CXXFLAG "-include")
endif()
target_compile_options(pyext_libopenage INTERFACE
	${FORCE_INCLUDE_CXXFLAG} "${CMAKE_SOURCE_DIR}/libopenage/pyinterface/hacks.h"
)
target_link_libraries(pyext_libopenage INTERFACE libopenage)
set(PYEXT_LINK_LIBRARY pyext_libopenage)

configure_file(run.py.in run.py)
add_cython_modules(EMBED NOINSTALL ${CMAKE_CURRENT_BINARY_DIR}/run.py)
add_py_modules(BININSTALL ${CMAKE_CURRENT_BINARY_DIR}/run.py AS openage)
add_subdirectory(openage/)

python_finalize()


##################################################
# packaging.

# Ensure that packaging is always the last step.
add_subdirectory(packaging)


##################################################
# show build configuration overview

message("")
print_config_options()

message("${PROJECT_NAME} ${PROJECT_VERSION}

   version string | ${VERSION_FULL_STRING}
         compiler | ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}
       build type | ${CMAKE_BUILD_TYPE}
         cxxflags | ${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS}
 build type flags | ${${BUILD_TYPE_CXX_FLAGS}}
        build dir | ${CMAKE_BINARY_DIR}
   install prefix | ${CMAKE_INSTALL_PREFIX}
py install prefix | ${CMAKE_PY_INSTALL_PREFIX}
")

##################################################
# done! that was easy, right?
