add_subdirectory("tests")

add_sources(libopenage
	tests.cpp
)

pxdgen(
	tests.h
)
