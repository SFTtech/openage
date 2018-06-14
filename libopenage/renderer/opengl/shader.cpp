// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#include "shader.h"

#include "../../datastructure/constexpr_map.h"


namespace openage {
namespace renderer {
namespace opengl {

static constexpr auto gl_shdr_type = datastructure::create_const_map<resources::shader_stage_t, GLenum>(
	std::make_pair(resources::shader_stage_t::vertex, GL_VERTEX_SHADER),
	std::make_pair(resources::shader_stage_t::geometry, GL_GEOMETRY_SHADER),
	std::make_pair(resources::shader_stage_t::tesselation_control, GL_TESS_CONTROL_SHADER),
	std::make_pair(resources::shader_stage_t::tesselation_evaluation, GL_TESS_EVALUATION_SHADER),
	std::make_pair(resources::shader_stage_t::fragment, GL_FRAGMENT_SHADER)
);

GlShader::GlShader(const resources::ShaderSource &src)
	: GlSimpleObject([] (GLuint handle) { glDeleteShader(handle); } )
	, type(gl_shdr_type.get(src.get_stage()))
{
	if (src.get_lang() != resources::shader_lang_t::glsl) {
		throw Error(MSG(err) << "Unsupported shader language passed to OpenGL renderer.");
	}

	// allocate shader in opengl
	GLuint handle = glCreateShader(this->type);
	this->handle = handle;

	// load shader source
	const char* data = src.get_source().c_str();
	glShaderSource(handle, 1, &data, 0);

	// compile shader source
	glCompileShader(handle);

	// check compiliation result
	GLint status;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		GLint loglen;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &loglen);

		std::vector<char> infolog(loglen);
		glGetShaderInfoLog(handle, loglen, 0, infolog.data());

		throw Error(MSG(err) << "Failed to compiler shader:\n" << infolog.data() );
	}
}

GLenum GlShader::get_type() const {
	return this->type;
}

}}} // openage::renderer::opengl
