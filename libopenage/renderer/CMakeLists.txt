add_sources(libopenage
  color.cpp
  geometry.cpp
  renderer.h
  shader_program.h
  tests.cpp
  text.cpp
  texture.cpp
  window.cpp
)

pxdgen(
	tests.h
)

add_subdirectory(font/)
add_subdirectory(opengl/)
add_subdirectory(resources/)
#add_subdirectory(shaders/)
