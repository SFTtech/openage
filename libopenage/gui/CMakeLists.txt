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
	gui_textured_points_shader.cpp
	main_args_link.cpp
	minimap_background.cpp
	minimap_marker_type.cpp
	minimap_unit_marker_points.cpp
	registrations.cpp
	resources_list_model.cpp
)

add_subdirectory("guisys")

add_sources(libopenage
	${QT_SDL_SOURCES}
)

add_subdirectory("integration")
