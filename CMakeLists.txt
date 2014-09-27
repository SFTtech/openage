cmake_minimum_required(VERSION 2.8.10)
#git tag --contains=fa7141f => 2.8.8    # compiler version detection
#git tag --contains=fbda7bb => 2.8.10   # find GLEW module

project(openage C CXX)

# main build configuration file

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
message("  *  IRC:      #sfttech  at freenode.org")
message("==============================================================================")
message("")

# include build configuration modules
include("buildsystem/python.cmake")
python_init()
include("buildsystem/cpp.cmake")
include("buildsystem/version.cmake")
include("buildsystem/doxygen.cmake")

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
