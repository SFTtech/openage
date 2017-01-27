# Copyright 2014-2017 the openage authors. See copying.md for legal info.

add_subdirectory(logo/)
add_subdirectory(shaders/)
add_subdirectory(textures/)


install(FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/missing.png"
	"${CMAKE_CURRENT_SOURCE_DIR}/gaben.png"
	DESTINATION "${INSTALLED_GLOBAL_ASSET_DIR}"
)
