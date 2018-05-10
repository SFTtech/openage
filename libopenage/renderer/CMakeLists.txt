add_sources(libopenage
  #camera.cpp
  color.cpp
  #game_renderer.cpp
  geometry.cpp
  renderer.h
  sdl_global.cpp
  shader_program.h
  #terrain_renderer.cpp
  tests.cpp
  batch_test.cpp
  text.cpp
  texture.cpp
  window.cpp

)

pxdgen(
	tests.h
  batch_test.h
)

add_subdirectory(font/)
add_subdirectory(opengl/)
add_subdirectory(resources/)
#add_subdirectory(vulkan/)
#add_subdirectory(shaders/)
