message("Cropping gaben.png to create placeholder desktop icon...")

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/dist")
execute_process(COMMAND python3 -c "
import PIL.Image;
cropped=PIL.Image.open('${GLOBAL_ASSET_DIR}/gaben.png').crop((92,137,604,649));
cropped.save('${CMAKE_BINARY_DIR}/dist/openage.png')")

install(FILES "${GLOBAL_ASSET_DIR}/dist/openage.desktop"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/share/applications")

install(FILES "${CMAKE_BINARY_DIR}/dist/openage.png"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/usr/share/pixmaps")
