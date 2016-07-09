add_sources(libopenage
	assetmanager_link.cpp
	category_contents_list_model.cpp
	engine_link.cpp
	game_control_link.cpp
	game_creator.cpp
	game_main_link.cpp
	game_saver.cpp
	game_spec_link.cpp
	generator_link.cpp
	input_context_recorder_player_link.cpp
	main_args_link.cpp
	resources_list_model.cpp
)

add_subdirectory("guisys")

add_sources(libopenage
	${QT_SDL_SOURCES}
)

add_subdirectory("integration")

install(DIRECTORY "qml"
	DESTINATION "${INSTALLED_GLOBAL_ASSET_DIR}"
)
