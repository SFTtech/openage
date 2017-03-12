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
#include "renderable.h"


namespace openage {
namespace renderer {
namespace opengl {

size_t uniform_size(gl_uniform_t type) {
	switch (type) {
	case gl_uniform_t::I32:
		return 4;
	case gl_uniform_t::U32:
		return 4;
	case gl_uniform_t::F32:
		return 4;
	case gl_uniform_t::F64:
		return 8;
	case gl_uniform_t::V2F32:
		return 8;
	case gl_uniform_t::V3F32:
		return 12;
	case gl_uniform_t::V4F32:
		return 16;
	case gl_uniform_t::TEX2D:
		return 4;
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
		return gl_uniform_t::TEX2D;
	else
		throw Error(MSG(err) << "Unsupported GLSL uniform type " << str);
}

void parse_glsl(std::unordered_map<std::string, gl_uniform_t> &uniforms, std::experimental::string_view code) {
	// this will match all uniform declarations
	std::regex unif_r("uniform\\s+\\w+\\s+\\w+(?=\\s*;)");
	std::regex const word_r("\\w+");

	std::smatch results;
	std::string s(code.data());
	if (regex_search(s, results, unif_r)) {
		for (std::string result : results) {
			// remove "uniform"
			result = result.substr(7);

			std::smatch words;
			regex_search(result, words, word_r);

			// first word is the type
			gl_uniform_t type = glsl_str_to_type(words.str(0));

			// second word is the uniform name
			uniforms.emplace(words.str(1), type);
		}
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
	GLuint id = glCreateShader(src_type_to_gl(src.type()));

	if (unlikely(id == 0)) {
		throw Error{MSG(err) << "Unable to create OpenGL shader. WTF?!", true};
	}

	// load shader source
	const char* data = src.source().data();
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
				return "[insert_task_here]";
			}
		}();

		throw Error(MSG(err) << "OpenGL shader program " << what_str << " failed:\n" << infolog.data(), true);
	}
}

GlShaderProgram::GlShaderProgram(const std::vector<resources::ShaderSource> &srcs) {
	this->id = glCreateProgram();

	if (unlikely(this->id == 0)) {
		throw Error(MSG(err) << "Unable to create OpenGL shader program. WTF?!");
	}

	std::unordered_map<std::string, gl_uniform_t> uniforms;
	std::vector<GLuint> shaders;
	for (auto src : srcs) {
		parse_glsl(uniforms, src.source());
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
	for (auto pair : uniforms) {
		GLint loc = glGetUniformLocation(this->id, pair.first.data());

		if (unlikely(loc == -1)) {
			throw Error(MSG(err) << "Could not determine location of OpenGL shader uniform that was found before. WTF?!");
		}

		GlUniform unif {
			pair.second,
			loc
		};

		this->uniforms.emplace(pair.first, unif);
	}

	// The uniform values are ordered in the byte buffer by however std::map orders their names
	size_t offset = 0;
	for (auto &pair : this->uniforms) {
		pair.second.offset = offset;
		offset += uniform_size(pair.second.type);
	}

	log::log(MSG(info) << "Created OpenGL shader program");
}

GlShaderProgram::~GlShaderProgram() {
	if (this->id != 0) {
		glDeleteProgram(this->id);
	}
}

GlShaderProgram::GlShaderProgram(GlShaderProgram &&other)
	: uniforms(std::move(other.uniforms))
	, id(other.id) {
	other.id = 0;
}

GlShaderProgram& GlShaderProgram::operator=(GlShaderProgram&& other) {
	this->uniforms = std::move(other.uniforms);
	this->id = other.id;
	other.id = 0;

	return *this;
}

void GlShaderProgram::use() const {
	glUseProgram(this->id);
}

void GlShaderProgram::execute_with(Renderable const& obj) {
	assert(obj.unif_in->program == this);


	auto const& unif_in = *static_cast<GlUniformInput const*>(obj.unif_in);
	uint8_t const* data = unif_in.update_data.data();

	for (auto pair : unif_in->update_offs) {
		uint8_t const* ptr = data + pair.second;
		auto loc = this->uniforms[pair.first].location;
		switch (this->uniforms[pair.first].type) {
		case gl_uniform_t::I32:
			glUniform1i(loc, *(int32_t*)ptr);
			break;
		case gl_uniform_t::U32:
			glUniform1ui(loc, *(uint32_t*)ptr);
			break;
		case gl_uniform_t::F32:
			glUniform1f(loc, *(float*)ptr);
			break;
		case gl_uniform_t::F64:
			// TODO requires an extension
			glUniform1d(loc, *(double*)ptr);
			break;
		case gl_uniform_t::V2F32:
			glUniform2fv(loc, 1, (float*)ptr);
			break;
		case gl_uniform_t::V3F32:
			glUniform3fv(loc, 1, (float*)ptr);
			break;
		case gl_uniform_t::V4F32:
			glUniform4fv(loc, 1, (float*)ptr);
			break;
		case gl_uniform_t::TEX2D:
			glUniform1i(loc, *(int32_t*)ptr);
			break;
		default:
			throw Error(MSG(err) << "Tried to upload unknown uniform type to GL shader.");
		}
	}

	// TODO read obj.geometry and obj.blend + family
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

std::unique_ptr<UniformInput> GlShaderProgram::new_unif_in() {
	GlUniformInput *in = new GlUniformInput;
	in->program = this;
	return std::unique_ptr<UniformInput>(in);
}

bool GlShaderProgram::has_unif(const char* name) {
	return this->uniforms.count(name) == 1;
}

void GlShaderProgram::set_unif(UniformInput *in, const char *unif, void const* val) {
	// will throw if uniform doesn't exist, that's ok
	// TODO rethrow with nicer message?
	auto const& unif_data = this->uniforms.at(unif);

	size_t size = uniform_size(unif_data.type);

	if (in->update_offs.count(unif) == 1) {
		// already wrote to this uniform since last upload
		size_t off = update_offs[unif];
		memcpy(in->update_data.data() + off, val, size);
	}
	else {
		size_t prev_size = in->update_data.size();
		in->update_data.resize(prev_size + size);
		memcpy(in->update_data.data() + prev_size, val, size);
		in->update_offs.emplace(unif, prev_size);
	}
}

void GlShaderProgram::set_i32(UnifomInput *in, const char *unif, int32_t val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_u32(UnifomInput *in, const char *unif, uint32_t val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_f32(UnifomInput *in, const char *unif, float val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_f64(UnifomInput *in, const char *unif, double val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_v2f32(UnifomInput *in, const char *unif, Eigen::Vector2f const& val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_v3f32(UnifomInput *in, const char *unif, Eigen::Vector3f const& val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_v4f32(UnifomInput *in, const char *unif, Eigen::Vector4f const& val) {
	this->set_unif(in, unif, &val);
}

void GlShaderProgram::set_tex(UnifomInput *in, const char *unif, Texture const* val) {
	// TODO special handling needed here
	throw "unimplemented";
}

}}} // openage::renderer::opengl
