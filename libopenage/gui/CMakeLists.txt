add_sources(libopenage
	assetmanager_link.cpp
	actions_list_model.cpp
	category_contents_list_model.cpp
	engine_info.cpp
	engine_link.cpp
	game_control_link.cpp
	game_creator.cpp
	game_main_link.cpp
	game_saver.cpp
	game_spec_link.cpp
	generator_link.cpp
	gui.cpp
	gui_cvar_list_model.cpp
	main_args_link.cpp
	registrations.cpp
	resources_list_model.cpp
)

add_subdirectory("guisys")

add_sources(libopenage
	${QT_SDL_SOURCES}
)

add_subdirectory("integration")
