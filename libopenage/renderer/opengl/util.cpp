// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "util.h"

#include "error/error.h"


namespace openage::renderer::opengl {

size_t get_uniform_type_size(GLenum type) {
	switch (type) {
	case GL_BOOL:
		return 1;
		break;
	case GL_BOOL_VEC2:
		return 2;
		break;
	case GL_BOOL_VEC3:
		return 3;
		break;
	case GL_BOOL_VEC4:
	case GL_FLOAT:
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
		return 4;
		break;

	case GL_FLOAT_VEC2:
	case GL_INT_VEC2:
	case GL_UNSIGNED_INT_VEC2:
		return 8;
		break;

	case GL_FLOAT_VEC3:
	case GL_INT_VEC3:
	case GL_UNSIGNED_INT_VEC3:
		return 12;
		break;

	case GL_FLOAT_VEC4:
	case GL_INT_VEC4:
	case GL_UNSIGNED_INT_VEC4:
	case GL_FLOAT_MAT2:
		return 16;
		break;

	case GL_FLOAT_MAT3:
		return 36;
		break;

	case GL_FLOAT_MAT4:
		return 64;
		break;

	default:
		throw Error(MSG(err) << "Unknown GL uniform type: " << type);
		break;
	}

	return 0;
}

} // namespace openage::renderer::opengl
