add_sources(${PROJECT_NAME}
	mapgen.cpp
	mapgenbase.cpp
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lnoise")

add_subdirectory(static)
add_subdirectory(diamondsquare)
add_subdirectory(mandelbrot)
add_subdirectory(perlinnoise)
