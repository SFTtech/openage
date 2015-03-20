cmake_minimum_required(VERSION 2.8.8)
#git tag --contains=fa7141f => 2.8.8    # compiler version detection

# main build configuration file

# text art: figlet -f rounded "[SFT] openage" | sed -e 's/\\/\\\\/g'
message("(running cmake...)

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

If it doesn't, consider reporting the issue/asking for help in #sfttech on freenode.net.
")

project(openage C CXX)

# options: keep up to date with those in ./configure!
if(NOT DEFINED WANT_INOTIFY)
	set(WANT_INOTIFY if_available)
endif()

if(NOT DEFINED WANT_GPERFTOOLS_PROFILER)
	set(WANT_GPERFTOOLS_PROFILER if_available)
endif()

if(NOT DEFINED WANT_GPERFTOOLS_TCMALLOC)
	set(WANT_GPERFTOOLS_TCMALLOC false)
endif()

set(BUILDSYSTEM_DIR "${CMAKE_SOURCE_DIR}/buildsystem")
set(CMAKE_MODULE_PATH "${BUILDSYSTEM_DIR}/modules/")

# include build configuration modules
include("CTest")

include("${BUILDSYSTEM_DIR}/util.cmake")

include("${BUILDSYSTEM_DIR}/python.cmake")
include("${BUILDSYSTEM_DIR}/cpp.cmake")
include("${BUILDSYSTEM_DIR}/codegen.cmake")

include("${BUILDSYSTEM_DIR}/assets.cmake")
include("${BUILDSYSTEM_DIR}/version.cmake")

include("${BUILDSYSTEM_DIR}/doxygen.cmake")

disallow_in_source_builds()

# create documentation for cpp and py folders
doxygen_configure(cpp/ openage/)

add_subdirectory(cpp/)
add_subdirectory(dist/)
add_subdirectory(openage/)

python_finalize()

print_config_options()

# show build configuration overview
message("${PROJECT_NAME} ${PROJECT_VERSION}

      compiler | ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}
      cxxflags | ${CMAKE_CXX_FLAGS}
install prefix | ${CMAKE_INSTALL_PREFIX}
     build dir | ${CMAKE_BINARY_DIR}
")

# detect missing run-time dependencies
warn_runtime_depends()
