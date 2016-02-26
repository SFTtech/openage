list(APPEND QT_SDL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/public/gui_application.cpp
)

list(APPEND QT_SDL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/private/gui_application_impl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/gui_dedicated_thread.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/gui_render_control.cpp
)

# TODO: platform check
list(APPEND QT_SDL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/private/platforms/context_extraction_x11.cpp
)

set(QT_SDL_SOURCES ${QT_SDL_SOURCES} PARENT_SCOPE)
