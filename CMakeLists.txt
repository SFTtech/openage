cmake_minimum_required(VERSION 2.8.10)
#git tag --contains=fa7141f => 2.8.8    # compiler version detection
#git tag --contains=fbda7bb => 2.8.10   # find GLEW module

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
If you have installed all the dependencies that are conveniently listed in [building.md], this _might_ work!

If it doesn't, try asking for help in #sfttech on freenode.net
")

project(openage C CXX)

set(BUILDSYSTEM_DIR "${CMAKE_SOURCE_DIR}/buildsystem")

# include build configuration modules
include("CTest")

include("${BUILDSYSTEM_DIR}/util.cmake")

include("${BUILDSYSTEM_DIR}/python.cmake")
include("${BUILDSYSTEM_DIR}/cpp.cmake")
include("${BUILDSYSTEM_DIR}/codegen.cmake")

include("${BUILDSYSTEM_DIR}/testing.cmake")
include("${BUILDSYSTEM_DIR}/doxygen.cmake")

include("${BUILDSYSTEM_DIR}/version.cmake")

# currently unused
detect_version()

# include source subdirectories
add_subdirectory("cpp")
add_subdirectory("py")

# process the included python sources, generate setup.py
process_python_modules()

# show build configuration overview
message("CFLAGS    = ${CMAKE_C_FLAGS}")
message("CXXFLAGS  = ${CMAKE_CXX_FLAGS}")
message("LDFLAGS   = ${CMAKE_EXE_LINKER_FLAGS}")
message("PREFIX    = ${CMAKE_INSTALL_PREFIX}")
message("BUILDDIR  = ${CMAKE_BINARY_DIR}")
