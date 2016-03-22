# Copyright 2016-2016 the openage authors. See copying.md for legal info.

# find the jsoncpp header
find_path(JSONCPP_INCLUDE_DIR
    NAMES jsoncpp/json/json.h json/json.h
    DOC "JsonCpp include directory"
)
mark_as_advanced(OPUS_INCLUDE_DIR)

# looking for jsoncpp lib
find_library(JSONCPP_LIBRARY
    NAMES jsoncpp
    DOC "Path to JsonCpp library"
)
mark_as_advanced(JSONCPP_LIBRARY)

MESSAGE( STATUS "JSONCPP_LIBRARY:         " ${JSONCPP_LIBRARY} )
MESSAGE( STATUS "JSON_INCLUDE_DIR:        " ${JSONCPP_INCLUDE_DIR} )

set(JSON_LIBRARIES "${JSONCPP_LIBRARY}")
set(JSON_INCLUDE_DIRS "${JSONCPP_INCLUDE_DIR}" "${JSONCPP_INCLUDE_DIR}/json")
