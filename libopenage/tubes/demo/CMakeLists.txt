set(CURSES_NEED_NCURSES TRUE)
find_package(Curses)

IF (${CURSES_FOUND})

	include_directories(${CURSES_INCLUDE_DIR})
	link_libraries (tube ${CURSES_LIBRARIES})

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -pedantic -Wall -Wextra")
	add_executable (tubepong
		main.cpp
		physics.cpp
		gui.cpp
		aicontroller.cpp
		)

	set_property(TARGET tubepong PROPERTY CXX_STANDARD 14)
ENDIF()

