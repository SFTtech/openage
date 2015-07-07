set(DIST_DIR "${CMAKE_SOURCE_DIR}/dist")

set(BIN_DIST_DIR "${CMAKE_BINARY_DIR}/dist")
file(MAKE_DIRECTORY "${BIN_DIST_DIR}")

set(GABEN_PNG "${GLOBAL_ASSET_DIR}/gaben.png")
set(OPENAGE_ICON "${BIN_DIST_DIR}/openage.png")


add_custom_command(
	OUTPUT "${OPENAGE_ICON}"
	COMMAND python3 -c "import PIL.Image; PIL.Image.open('${GABEN_PNG}').crop((92,137,604,649)).save('${OPENAGE_ICON}')"
	DEPENDS "${GABEN_PNG}"
	VERBATIM
	COMMENT "cropping gaben.png to create placeholder desktop icon")

add_custom_target(distfiles ALL DEPENDS "${OPENAGE_ICON}")


install(
	FILES "${DIST_DIR}/openage.desktop"
	DESTINATION "share/applications"
)

install(
	FILES "${OPENAGE_ICON}"
	DESTINATION "share/pixmaps"
)
