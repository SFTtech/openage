add_sources(libopenage
	actions_list_model.cpp
	gui.cpp
	registrations.cpp
)

add_subdirectory("guisys")

add_sources(libopenage
	${QT_SDL_SOURCES}
)

add_subdirectory("integration")
