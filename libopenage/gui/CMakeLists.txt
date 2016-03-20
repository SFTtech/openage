add_sources(libopenage
	assetmanager_link.cpp
	engine_link.cpp
	game_creator.cpp
	game_main_link.cpp
	game_saver.cpp
	game_spec_link.cpp
	generator_link.cpp
	main_args_link.cpp
)

add_subdirectory("guisys")

add_sources(libopenage
	${QT_SDL_SOURCES}
)

add_subdirectory("integration")
