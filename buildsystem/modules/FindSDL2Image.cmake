find_package(PackageHandleStandardArgs)

if(APPLE)
	find_library(SDL2IMAGE_LIBRARIES SDL2_image DOC "SDL2 library framework for MacOS X")
	find_path(SDL2IMAGE_INCLUDE_DIRS SDL2_image/SDL_image.h DOC "Include directory for SDL2_image under MacOS X")
else()
	find_library(SDL2IMAGE_LIBRARIES SDL2_image DOC "SDL2 library")
	find_path(SDL2IMAGE_INCLUDE_DIRS SDL2/SDL_image.h DOC "Include directory for SDL2_image")
endif()

# handle the QUIETLY and REQUIRED arguments and set OPENGL_FOUND to TRUE if
# all listed variables are TRUE
find_package_handle_standard_args(SDL2 DEFAULT_MSG SDL2IMAGE_LIBRARIES)
