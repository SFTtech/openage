add_sources(libopenage
  color.cpp
  geometry.cpp
  sdl_global.cpp
  tests.cpp
  text.cpp
  texture.cpp
  texture_array.cpp
  window.cpp
)

pxdgen(
	tests.h
)

add_subdirectory(font/)
add_subdirectory(opengl/)
add_subdirectory(resources/)
#add_subdirectory(vulkan/)
