# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# This module defines
#
# INOTIFY_INCLUDE_DIR
# INOTIFY_FOUND

find_path(INOTIFY_INCLUDE_DIR sys/inotify.h HINTS /usr/include/${CMAKE_LIBRARY_ARCHITECTURE})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(inotify DEFAULT_MSG INOTIFY_INCLUDE_DIR)

mark_as_advanced(INOTIFY_INCLUDE_DIR)
