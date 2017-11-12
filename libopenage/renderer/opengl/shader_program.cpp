// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "../../config.h"

#include "shader_program.h"

#include <experimental/string_view>
#include <regex>
#include <istream>
#include <functional>

#include "../../error/error.h"
#include "../../log/log.h"
#include "../../util/compiler.h"
#include "../../util/file.h"
#include "../../util/strings.h"
#include "texture.h"
#include "geometry.h"


namespace openage {
namespace renderer {
namespace opengl {

size_t uniform_size(gl_uniform_t type) {
	switch (type) {
	case gl_uniform_t::I32:
		return sizeof(GLint);
	case gl_uniform_t::U32:
		return sizeof(GLuint);
	case gl_uniform_t::F32:
		return sizeof(float);
	case gl_uniform_t::F64:
		return sizeof(double);
	case gl_uniform_t::V2F32:
		return 2 * sizeof(float);
	case gl_uniform_t::V3F32:
		return 3 * sizeof(float);
	case gl_uniform_t::V4F32:
		return 4 * sizeof(float);
	case gl_uniform_t::M4F32:
		return 16 * sizeof(float);
	case gl_uniform_t::SAMPLER2D:
		return sizeof(GLint);
	default:
		throw Error(MSG(err) << "Tried to find size of unknown GL uniform type.");
	}
}

static gl_uniform_t glsl_str_to_type(std::experimental::string_view str) {
	if (str == "int")
		return gl_uniform_t::I32;
	else if (str == "uint")
		return gl_uniform_t::U32;
	else if (str == "float")
		return gl_uniform_t::F32;
	else if (str == "double")
		return gl_uniform_t::F64;
	else if (str == "vec2")
		return gl_uniform_t::V2F32;
	else if (str == "vec3")
		return gl_uniform_t::V3F32;
	else if (str == "vec4")
		return gl_uniform_t::V4F32;
	else if (str == "sampler2D")
		return gl_uniform_t::SAMPLER2D;
	else if (str == "mat4")
		return gl_uniform_t::M4F32;
	else
		throw Error(MSG(err) << "Unsupported GLSL uniform type " << str);
}

void parse_glsl(std::map<std::string, GlUniform> &uniforms, const char *code) {
	// this will match all uniform declarations
	std::regex const unif_r("uniform\\s+\\w+\\s+\\w+(?=\\s*;)");
	std::regex const word_r("\\w+");

	const char *end = code;
	while (*end != '\0') {
		end += 1;
	}

	auto unif_iter = std::cregex_iterator(code, end, unif_r);
	auto unif_iter_end = std::cregex_iterator();
	for (; unif_iter != unif_iter_end; unif_iter++) {
		std::string unif = (*unif_iter).str();

		// remove "uniform"
		unif = unif.substr(7);

		auto word_iter = std::sregex_iterator(unif.begin(), unif.end(), word_r);

		// first word is the type
		gl_uniform_t type = glsl_str_to_type((*word_iter).str());

		// second word is the uniform name
		word_iter++;

		// second word is the uniform name
		uniforms.insert(std::pair<std::string, GlUniform>(
			                (*word_iter).str(),
			                GlUniform {
				                type,
				                0,
				            }
		                ));

	}
}

static GLuint src_type_to_gl(resources::shader_source_t type) {
	using resources::shader_source_t;

	switch (type) {
	case shader_source_t::glsl_vertex:
		return GL_VERTEX_SHADER;
	case shader_source_t::glsl_geometry:
		return GL_GEOMETRY_SHADER;
	case shader_source_t::glsl_tesselation_control:
		return GL_TESS_CONTROL_SHADER;
	case shader_source_t::glsl_tesselation_evaluation:
		return GL_TESS_EVALUATION_SHADER;
	case shader_source_t::glsl_fragment:
		return GL_FRAGMENT_SHADER;
	default:
		throw Error(MSG(err) << "Unsupported GLSL shader type.");
	}
}

static GLuint compile_shader(const resources::ShaderSource& src) {
	// allocate shader in opengl
	GLuint id = glCreateShader(src_type_to_gl(src.get_type()));

	if (unlikely(id == 0)) {
		throw Error{MSG(err) << "Unable to create OpenGL shader. WTF?!", true};
	}

	// load shader source
	const char* data = src.get_source();
	glShaderSource(id, 1, &data, 0);

	// compile shader source
	glCompileShader(id);

	// check compiliation result
	GLint status;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		GLint loglen;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &loglen);

		std::vector<char> infolog(loglen);
		glGetShaderInfoLog(id, loglen, 0, infolog.data());

		auto errmsg = MSG(err);
		errmsg << "Failed to compile shader:\n" << infolog.data();

		glDeleteShader(id);

		throw Error{errmsg, true};
	}

	return id;
}

static void check_program_status(GLuint program, GLenum what_to_check) {
	GLint status;
	glGetProgramiv(program, what_to_check, &status);

	if (status != GL_TRUE) {
		GLint loglen;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglen);

		std::vector<char> infolog(loglen);
		glGetProgramInfoLog(program, loglen, 0, infolog.data());

		const char *what_str = [=] {
			switch (what_to_check) {
			case GL_LINK_STATUS:
				return "linking";
			case GL_VALIDATE_STATUS:
				return "validation";
			case GL_COMPILE_STATUS:
				return "compilation";
			default:
				return "unknown shader creation task";
			}
		}();

		throw Error(MSG(err) << "OpenGL shader program " << what_str << " failed:\n" << infolog.data(), true);
	}
}

GlShaderProgram::GlShaderProgram(const std::vector<resources::ShaderSource> &srcs, const gl_context_capabilities &caps) {
	this->id = glCreateProgram();

	if (unlikely(this->id == 0)) {
		throw Error(MSG(err) << "Unable to create OpenGL shader program. WTF?!");
	}

	std::vector<GLuint> shaders;
	for (auto src : srcs) {
		parse_glsl(this->uniforms, src.get_source());
		shaders.push_back(compile_shader(src));
	}

	for (GLuint shdr : shaders) {
		glAttachShader(this->id, shdr);
	}

	glLinkProgram(this->id);
	check_program_status(this->id, GL_LINK_STATUS);

	glValidateProgram(this->id);
	check_program_status(this->id, GL_VALIDATE_STATUS);

	// after linking we can delete the shaders
	for (GLuint shdr : shaders) {
		glDetachShader(this->id, shdr);
		glDeleteShader(shdr);
	}

	// find the location of every uniform in the shader program
	for (auto& pair : this->uniforms) {
		GLint loc = glGetUniformLocation(this->id, pair.first.data());

		pair.second.location = loc;

		if (loc == -1) {
			log::log(MSG(info)
			            << "Could not determine the location of OpenGL shader uniform that was found before. Probably optimized away.");
			continue;
		}

		GLuint tex_unit = 0;
		if (pair.second.type == gl_uniform_t::SAMPLER2D) {
			if (tex_unit >= caps.max_texture_slots) {
				throw Error(MSG(err)
				            << "Tried to create shader that uses more texture sampler uniforms"
				            << "than there are texture unit slots available.");
			}
			this->texunits_per_unifs.insert(std::make_pair(pair.first, tex_unit));
			tex_unit += 1;
		}

	}

	log::log(MSG(info) << "Created OpenGL shader program");
}

GlShaderProgram::GlShaderProgram(GlShaderProgram &&other)
	: uniforms(std::move(other.uniforms))
	, id(other.id) {
	other.id = 0;
}

GlShaderProgram& GlShaderProgram::operator=(GlShaderProgram&& other) {
	if (this->id != 0) {
		glDeleteProgram(this->id);
	}

	this->uniforms = std::move(other.uniforms);
	this->id = other.id;
	other.id = 0;

	return *this;
}

GlShaderProgram::~GlShaderProgram() {
	if (this->id != 0) {
		glDeleteProgram(this->id);
	}
}

void GlShaderProgram::use() const {
	glUseProgram(this->id);

	for (auto const &pair : this->textures_per_texunits) {
		// We have to bind the texture to their texture units here because
		// the texture unit bindings are global to the context. Each time
		// the shader switches, it is possible that some other shader overwrote
		// these, and since we want the uniform values to persist across execute_with
		// calls, we have to set them more often than just on execute_with.
		glActiveTexture(GL_TEXTURE0 + pair.first);
		glBindTexture(GL_TEXTURE_2D, pair.second);
	}
}

void GlShaderProgram::execute_with(const GlUniformInput *unif_in, const Geometry *geom) {
	assert(unif_in->program == this);

	this->use();

	uint8_t const* data = unif_in->update_data.data();
	for (auto const &pair : unif_in->update_offs) {
		uint8_t const* ptr = data + pair.second;
		auto loc = this->uniforms[pair.first].location;
		if(loc == -1) {
			continue;
		}
		switch (this->uniforms[pair.first].type) {
		case gl_uniform_t::I32:
			glUniform1i(loc, *reinterpret_cast<const GLint*>(ptr));
			break;
		case gl_uniform_t::U32:
			glUniform1ui(loc, *reinterpret_cast<const GLuint*>(ptr));
			break;
		case gl_uniform_t::F32:
			glUniform1f(loc, *reinterpret_cast<const float*>(ptr));
			break;
		case gl_uniform_t::F64:
			// TODO requires an extension
			glUniform1d(loc, *reinterpret_cast<const double*>(ptr));
			break;
		case gl_uniform_t::V2F32:
			glUniform2fv(loc, 1, reinterpret_cast<const float*>(ptr));
			break;
		case gl_uniform_t::V3F32:
			glUniform3fv(loc, 1, reinterpret_cast<const float*>(ptr));
			break;
		case gl_uniform_t::V4F32:
			glUniform4fv(loc, 1, reinterpret_cast<const float*>(ptr));
			break;
		case gl_uniform_t::M4F32:
			glUniformMatrix4fv(loc, 1, false, reinterpret_cast<const float*>(ptr));
			break;
		case gl_uniform_t::SAMPLER2D: {
			GLuint tex_unit = this->texunits_per_unifs[pair.first];
			GLuint tex = *reinterpret_cast<const GLuint*>(ptr);
			glActiveTexture(GL_TEXTURE0 + tex_unit);
			glBindTexture(GL_TEXTURE_2D, tex);
			//TODO: maybe call this at an more appropiate position
			glUniform1i(loc, tex_unit);
			this->textures_per_texunits[tex_unit] = tex;
			break;
		}
		default:
			throw Error(MSG(err) << "Tried to upload unknown uniform type to GL shader.");
		}
	}

	if (geom != nullptr) {
		// TODO read geom and obj.blend + family
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		geom->draw();
	}
}

std::unique_ptr<UniformInput> GlShaderProgram::new_unif_in() {
	GlUniformInput *in = new GlUniformInput;
	in->program = this;
	return std::unique_ptr<UniformInput>(in);
}

bool GlShaderProgram::has_uniform(const char* name) {
	return this->uniforms.count(name) == 1;
}

void GlShaderProgram::set_unif(UniformInput *in, const char *unif, void const* val) {
	GlUniformInput *unif_in = static_cast<GlUniformInput*>(in);
	// will throw if uniform doesn't exist, that's ok
	// TODO rethrow with nicer message?
	auto const& unif_data = this->uniforms.at(unif);

	size_t size = uniform_size(unif_data.type);

	if (unif_in->update_offs.count(unif) == 1) {
		// already wrote to this uniform since last upload
		size_t off = unif_in->update_offs[unif];
		memcpy(unif_in->update_data.data() + off, val, size);
	}
	else {
		size_t prev_size = unif_in->update_data.size();
		unif_in->update_data.resize(prev_size + size);
		memcpy(unif_in->update_data.data() + prev_size, val, size);
		unif_in->update_offs.emplace(unif, prev_size);
	}
}

void GlShaderProgram::set_i32(UniformInput *in, const char *unif, int32_t val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_u32(UniformInput *in, const char *unif, uint32_t val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_f32(UniformInput *in, const char *unif, float val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_f64(UniformInput *in, const char *unif, double val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_v2f32(UniformInput *in, const char *unif, Eigen::Vector2f const& val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_v3f32(UniformInput *in, const char *unif, Eigen::Vector3f const& val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_v4f32(UniformInput *in, const char *unif, Eigen::Vector4f const& val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_m4f32(UniformInput *in, const char *unif, Eigen::Matrix4f const& val) {
	this->set_unif(in, unif, val.data());
}

void GlShaderProgram::set_tex(UniformInput *in, const char *unif, Texture const* val) {
	auto const& tex = *static_cast<const GlTexture*>(val);
	GLuint handle = tex.get_handle();
	this->set_unif(in, unif, &handle);
}

}}} // openage::renderer::opengl
