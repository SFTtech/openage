# Copyright 2014-2017 the openage authors. See copying.md for legal info.

add_subdirectory(logo/)
add_subdirectory(shaders/)
add_subdirectory(textures/)


install(FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/missing.png"
	"${CMAKE_CURRENT_SOURCE_DIR}/gaben.png"
	DESTINATION "${INSTALLED_GLOBAL_ASSET_DIR}"
)

install(DIRECTORY "qml"
	DESTINATION "${INSTALLED_GLOBAL_ASSET_DIR}"
)

# To show QML files in the QtCreator.
file(GLOB QML_SRC "qml/*.qml")
add_custom_target(qtcreator-show-qml SOURCES ${QML_SRC})
