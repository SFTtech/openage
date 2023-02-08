add_sources(libopenage
	assetmanager_link.cpp
	engine_link.cpp
	gui.cpp
	qml_info.cpp
)

add_subdirectory("guisys")
add_subdirectory("integration")

add_sources(libopenage
	${QTGUI_SOURCES}
)
