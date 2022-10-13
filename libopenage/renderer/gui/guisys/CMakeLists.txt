list(APPEND QTGUI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/private/gui_ctx_setup.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/gui_renderer_impl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/gui_rendering_setup_routines.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/opengl_debug_logger.cpp
)

list(APPEND QTGUI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/public/gui_renderer.cpp
)

set(QTGUI_SOURCES ${QTGUI_SOURCES} PARENT_SCOPE)
