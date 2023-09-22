// Copyright 2013-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <initializer_list>

#include <epoxy/gl.h>

namespace openage {
namespace shader {

[[deprecated]] const char *type_to_string(GLenum type);

class [[deprecated]] Shader {
public:
	Shader(GLenum type, std::initializer_list<const char *> sources);
	~Shader();

	GLuint id;
	GLenum type;
};

} // namespace shader
} // namespace openage
