# Copyright 2020-2020 the openage authors. See copying.md for legal info.

# provides global targets for platform dependencies

#################################################
# Minimum version requirements
set(PYTHON_MIN_VERSION 3.6)
set(CYTHON_MIN_VERSION 0.25)
set(QT_VERSION_REQ "5.9")
set(EIGEN_VERSION_REQ "3.3")
set(HARFBUZZ_VERSION_REQ "1.0.0")


##################################################
# nyan integration
include(HandleExternalNyan)


#################################################
# Find packages

# Handles Python and it's runtime dependencies
include(ProvidePython)

# We use these modules
set(REQUIRED_PYTHON_MODULES "PIL.Image" "PIL.ImageDraw" "numpy" "pygments" "jinja2")

# Check for these modules
include(CheckPythonModules)

# Find package
find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2Image REQUIRED)
find_package(Opusfile REQUIRED)
find_package(Epoxy REQUIRED)
find_package(HarfBuzz ${HARFBUZZ_VERSION_REQ} REQUIRED)
find_package(Eigen3 ${EIGEN_VERSION_REQ} REQUIRED NO_MODULE)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

# provide apple qt5 location
# TODO: Check if pkg-config is fine
# => DELETE
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH /usr/local/opt/qt5)
endif()

if(MINGW)
    find_package(Qt5 ${QT_VERSION_REQ}
            COMPONENTS
            REQUIRED
                Core
                Quick
                QuickControls2
    )
elseif(NOT MINGW)
    find_package(Qt5Core ${QT_VERSION_REQ} REQUIRED)
    find_package(Qt5Quick ${QT_VERSION_REQ} REQUIRED)
endif()

##################################################
# library dependency specification
# platform specifics

# windows does not have libm
if(NOT WIN32)
    find_library(MATH_LIB m)
    find_library(UTIL_LIB util)
endif()

if(NOT APPLE AND NOT WIN32)
    find_library(RT_LIB rt)
    if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
        find_library(EXECINFO_LIB execinfo)
    endif()
endif()

# DEBUG
if(NOT DEFINED ${DEBUG_VERBOSITY})
    set(PKG_CHECK_QUIET "QUIET")
else()
    set(PKG_CHECK_QUIET "")
endif()

###################################################
# ALL PLATFORMS
pkg_check_modules(PLATFORM_PYTHON3
        "${PKG_CHECK_QUIET}"
        IMPORTED_TARGET
        GLOBAL
        python3>=3.6
)

pkg_check_modules(PLATFORM_SDL2
        "${PKG_CHECK_QUIET}"
        IMPORTED_TARGET
        GLOBAL
            sdl2
)

pkg_check_modules(PLATFORM_FONT
        "${PKG_CHECK_QUIET}"
        IMPORTED_TARGET
        GLOBAL
            freetype2
            fontconfig
            harfbuzz
)

pkg_check_modules(PLATFORM_GRAPHIC
        "${PKG_CHECK_QUIET}"
        IMPORTED_TARGET
        GLOBAL
            SDL2_image
            epoxy
            libpng
)


if(WIN32)
###################################################
# WINDOWS, MINGW specific

    pkg_check_modules(PLATFORM_AUDIO
            "${PKG_CHECK_QUIET}"
            IMPORTED_TARGET
            GLOBAL
                ogg
                opus
                opusfile
    )

    target_link_libraries(libopenage
            PRIVATE
                DbgHelp
    )


elseif(APPLE)
###################################################
# APPLE specific
# TODO: Check if pkg-config is fine
# => DELETE
    find_path(QTPLATFORM_INCLUDE_DIRS QtPlatformHeaders "/usr/local/opt/qt5/include")

else()
###################################################
# LINUX, UNIX, etc. specific

    pkg_check_modules(PLATFORM_AUDIO
            "${PKG_CHECK_QUIET}"
            IMPORTED_TARGET
            GLOBAL
                opus
                opusfile
    )
endif()

##################################################
# optional dependencies

# GCCBacktrace
# advanced stacktraces with libbacktrace from gcc
if(WANT_BACKTRACE)
    find_package(GCCBacktrace)
    if(GCCBacktrace_FOUND)
        have_config_option(backtrace BACKTRACE true)

        target_include_directories(libopenage
                PRIVATE
                ${GCCBacktrace_INCLUDE_DIRS}
                )

        target_link_libraries(libopenage
                PRIVATE
                "${GCCBacktrace_LIBRARIES}"
                )

    else()
        have_config_option(backtrace BACKTRACE false)
    endif()
endif()

# google performance tools
if(WANT_GPERFTOOLS_PROFILER OR WANT_GPERFTOOLS_TCMALLOC)
    find_package(GPerfTools)
    if(GPERFTOOLS_FOUND)
        # GPERFTOOLS PROFILER
        if(WANT_GPERFTOOLS_PROFILER)
            have_config_option(gperftools-profiler GPERFTOOLS_PROFILER true)

            target_include_directories(libopenage
                    PRIVATE
                    ${GPERFTOOLS_INCLUDE_DIR}
                    )

            target_link_libraries(libopenage
                    PRIVATE
                    ${GPERFTOOLS_PROFILER}
                    )

        endif()

        #GPERFTOOLS TCMALLOC
        if(WITH_GPERFTOOLS_TCMALLOC)
            have_config_option(gperftools-tcmalloc GPERFTOOLS_TCMALLOC true)

            target_include_directories(libopenage
                    PRIVATE
                    ${GPERFTOOLS_INCLUDE_DIR}
                    )

            target_link_libraries(libopenage
                    PRIVATE
                    ${GPERFTOOLS_TCMALLOC}
                    )

        endif()
    else()
        have_config_option(gperftools-tcmalloc GPERFTOOLS_TCMALLOC false)
        have_config_option(gperftools-profiler GPERFTOOLS_PROFILER false)
    endif()
endif()

# inotify support
if(WANT_INOTIFY)
    find_package(Inotify)

    if(INOTIFY_FOUND)
        have_config_option(inotify INOTIFY true)

        target_include_directories(libopenage
                PRIVATE
                ${INOTIFY_INCLUDE_DIRS}
                )

    else()
        have_config_option(inotify INOTIFY false)
    endif()
endif()

# ncurses support
if(WANT_NCURSES)
    set(CURSES_NEED_NCURSES TRUE)
    find_package(Curses)

    if(CURSES_FOUND)
        have_config_option(ncurses NCURSES true)

        target_include_directories(libopenage
                PRIVATE
                ${Curses_INCLUDE_DIRS}
                )

        target_link_libraries(libopenage
                PRIVATE
                ${CURSES_LIBRARIES}
                )

    else()
        have_config_option(ncurses NCURSES false)
    endif()
endif()

# opengl support
if(WANT_OPENGL)
    find_package(OpenGL)

    if(OPENGL_FOUND)
        have_config_option(opengl OPENGL true)

        target_link_libraries(libopenage
                PRIVATE
                OpenGL::GL
                )

    else()
        have_config_option(opengl OPENGL false)
    endif()
endif()

# vulkan support
if(WANT_VULKAN)
    find_package(Vulkan)

    if(VULKAN_FOUND)
        have_config_option(vulkan VULKAN true)

        target_link_libraries(libopenage
                PRIVATE
                Vulkan::Vulkan
                )

    else()
        have_config_option(vulkan VULKAN false)
    endif()
endif()

if(NOT (OPENGL_FOUND OR VULKAN_FOUND))
    message(FATAL_ERROR "One of OpenGL or Vulkan is required!")
endif()
