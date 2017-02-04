add_sources(libopenage
	filelike.cpp
	filelike_stdlib.cpp
	filelike_python.cpp
)

pxdgen(
	filelike.h
	filelike_python.h
)
