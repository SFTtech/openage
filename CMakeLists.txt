cmake_minimum_required(VERSION 2.8.10)
#git tag --contains=fa7141f => 2.8.8    # compiler version detection
#git tag --contains=fbda7bb => 2.8.10   # find GLEW module

project(openage C CXX)

# main build configuration file

# text art: figlet -f rounded "[SFT] openage" | sed -e 's/\\/\\\\/g'
message("")
message("==============================================================================")
message("  ___  ______ _______ _______ ___")
message("|  _)/ _____|_______|_______|_  |")
message("| | ( (____  _____      _     | |    ___  ____  _____ ____  _____  ____ _____")
message("| |  \\____ \\|  ___)    | |    | |   / _ \\|  _ \\| ___ |  _ \\(____ |/ _  | ___ |")
message("| |_ _____) ) |        | |   _| |  | |_| | |_| | ____| | | / ___ ( (_| | ____|")
message("|___|______/|_|        |_|  (___|   \\___/|  __/|_____)_| |_\\_____|\\___ |_____)")
message("                                         |_|                     (_____|")
message("")
message(" Welcome to the SFT technologies computer aided openage build system!")
message(" You have chosen, or been chosen, to do the adventurous task of")
message("  compiling openage.")
message(" If you payed attention to the dependency requirements,")
message("  the build process should just work!")
message("")
message(" We hope your expectations will not be disappointed,")
message("  otherwise, contact our support team and the developers:")
message("  *  IRC:      #sfttech  at freenode.org")
message("==============================================================================")
message("")

# include build configuration modules
include("CTest")
include("buildsystem/python.cmake")
python_init()
include("buildsystem/cpp.cmake")
include("buildsystem/version.cmake")
include("buildsystem/doxygen.cmake")
include("buildsystem/testing.cmake")

# include source subdirectories
add_subdirectory("src")
add_subdirectory("convert")

# process the included python sources, generate setup.py
process_python_modules()

# generate Makefile
configure_file(${CMAKE_SOURCE_DIR}/Makefile.in ${CMAKE_SOURCE_DIR}/Makefile @ONLY)

# show build configuration overview
message("CFLAGS    = ${CMAKE_C_FLAGS}")
message("CXXFLAGS  = ${CMAKE_CXX_FLAGS}")
message("LDFLAGS   = ${CMAKE_EXE_LINKER_FLAGS}")
message("PREFIX    = ${CMAKE_INSTALL_PREFIX}")
message("BUILDDIR  = ${CMAKE_BINARY_DIR}")
# TODO add more messages
