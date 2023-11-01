list(APPEND QT_SDL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/link/gui_item.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/link/gui_list_model.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/link/gui_property_map_impl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/link/gui_singleton_item.cpp
)

list(APPEND QTGUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/private/livereload/deferred_initial_constant_property_values.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/livereload/gui_live_reloader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/livereload/recursive_directory_watcher_worker.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/livereload/recursive_directory_watcher.cpp
)

list(APPEND QTGUI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/private/gui_application_impl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/gui_ctx_setup.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/gui_engine_impl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/gui_input_impl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/gui_renderer_impl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/gui_rendering_setup_routines.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/gui_subtree_impl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/private/opengl_debug_logger.cpp
)

list(APPEND QTGUI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/public/gui_application.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/public/gui_engine.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/public/gui_input.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/public/gui_renderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/public/gui_subtree.cpp
)

set(QTGUI_SOURCES ${QTGUI_SOURCES} PARENT_SCOPE)
