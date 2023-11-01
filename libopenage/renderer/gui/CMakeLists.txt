add_sources(libopenage
	gui.cpp
)

add_subdirectory("guisys")
add_subdirectory("integration")

add_sources(libopenage
	${QTGUI_SOURCES}
)
