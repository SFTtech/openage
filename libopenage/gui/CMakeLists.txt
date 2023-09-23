add_sources(libopenage
	actions_list_model.cpp
	category_contents_list_model.cpp
	game_creator.cpp
	game_saver.cpp
	game_spec_link.cpp
	generator_link.cpp
	gui.cpp
	registrations.cpp
	resources_list_model.cpp
)

add_subdirectory("guisys")

add_sources(libopenage
	${QT_SDL_SOURCES}
)

add_subdirectory("integration")
