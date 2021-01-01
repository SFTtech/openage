# Copyright 2017-2021 the openage authors. See copying.md for legal info.

install(FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/keybinds.oac"
	DESTINATION "${GLOBAL_CONFIG_DIR}"
)

install(DIRECTORY
	"${CMAKE_CURRENT_SOURCE_DIR}/converter"
	DESTINATION "${GLOBAL_CONFIG_DIR}/"
)
