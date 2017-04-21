add_sources(libopenage
  tests.cpp
  texture.cpp
  window.cpp
  text.cpp
  color.cpp
)

pxdgen(
	tests.h
)

add_subdirectory(font/)
add_subdirectory(opengl/)
add_subdirectory(resources/)
#add_subdirectory(shaders/)
