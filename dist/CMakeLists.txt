set(DIST_DIR "${CMAKE_SOURCE_DIR}/dist")

set(BIN_DIST_DIR "${CMAKE_BINARY_DIR}/dist")
file(MAKE_DIRECTORY "${BIN_DIST_DIR}")

set(GABEN_PNG "${GLOBAL_ASSET_DIR}/gaben.png")
set(OPENAGE_ICON "${BIN_DIST_DIR}/openage.png")


add_custom_command(
	OUTPUT "${OPENAGE_ICON}"
	COMMAND python3 -c "
import PIL.Image;
cropped=PIL.Image.open('${GABEN_PNG}').crop((92,137,604,649));
cropped.save('${OPENAGE_ICON}')"
	DEPENDS "${GABEN_PNG}"
	COMMENT "cropping gaben.png to create placeholder desktop icon")


install(FILES "${DIST_DIR}/openage.desktop"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications")

install(FILES "${OPENAGE_ICON}"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps")
