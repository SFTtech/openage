configure_file(compiletime.h.in compiletime.h)
configure_file(compiletime.cpp.in compiletime.cpp)

add_sources(libopenage
	${CMAKE_CURRENT_BINARY_DIR}/compiletime.cpp
	versions.cpp
)

pxdgen(
	versions.h
)
