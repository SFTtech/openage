function(init_assets)
	set(ASSET_DIR ${CMAKE_SOURCE_DIR}/data)

	install(DIRECTORY ${ASSET_DIR}/ DESTINATION "${CMAKE_INSTALL_PREFIX}/share/openage"
		PATTERN .gitignore EXCLUDE)

	set(ASSET_DIR ${ASSET_DIR} PARENT_SCOPE)
endfunction()

init_assets()
