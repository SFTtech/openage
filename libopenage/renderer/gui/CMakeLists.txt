add_sources(libopenage
	engine_link.cpp
	gui.cpp
	qml_info.cpp
)

add_subdirectory("guisys")
add_subdirectory("integration")

add_sources(libopenage
	${QTGUI_SOURCES}
)
