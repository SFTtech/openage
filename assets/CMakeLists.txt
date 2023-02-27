# Copyright 2014-2017 the openage authors. See copying.md for legal info.

add_subdirectory(logo/)
add_subdirectory(shaders/)
add_subdirectory(test/)
add_subdirectory(textures/)

install(DIRECTORY "qml"
	DESTINATION "${ASSET_DIR}"
)

# To show QML files in the QtCreator.
file(GLOB QML_SRC "qml/*.qml")
add_custom_target(qtcreator-show-qml SOURCES ${QML_SRC})
