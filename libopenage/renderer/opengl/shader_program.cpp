// Copyright 2013-2018 the openage authors. See copying.md for legal info.

#include "shader_program.h"

#include "../../error/error.h"
#include "../../log/log.h"
#include "../../datastructure/constexpr_map.h"

#include "texture.h"
#include "shader.h"
#include "geometry.h"
#include "lookup.h"


namespace openage {
namespace renderer {
namespace opengl {

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

GlShaderProgram::GlShaderProgram(const std::vector<resources::ShaderSource> &srcs, const gl_context_capabilities &caps)
	: GlSimpleObject([] (GLuint handle) { glDeleteProgram(handle); } ) {
	GLuint handle = glCreateProgram();
	this->handle = handle;

	std::vector<GlShader> shaders;
	for (auto src : srcs) {
		GlShader shader(src);
		glAttachShader(handle, shader.get_handle());
		shaders.push_back(std::move(shader));
	}

	glLinkProgram(handle);
	check_program_status(handle, GL_LINK_STATUS);

	glValidateProgram(handle);
	check_program_status(handle, GL_VALIDATE_STATUS);

	// after linking we can delete the shaders
	for (auto const& shdr : shaders) {
		glDetachShader(handle, shdr.get_handle());
	}

	// query program information
	GLint val;
	glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &val);
	size_t attrib_count = val;
	glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &val);
	size_t attrib_maxlen = val;
	glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &val);
	size_t unif_count = val;
	glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &val);
	size_t unif_maxlen = val;

	std::vector<char> name(std::max(unif_maxlen, attrib_maxlen));

	GLuint tex_unit = 0;
	for (GLuint i_unif = 0; i_unif < unif_count; ++i_unif) {
		GLint count;
		GLenum type;
		glGetActiveUniform(
			handle,
			i_unif,
			name.size(),
			0,
			&count,
			&type,
			name.data()
		);

		this->uniforms.insert(std::make_pair(
			                      name.data(),
			                      GlUniform {
				                      type,
				                      GLint(i_unif),
				                      size_t(count),
				                      size_t(count) * GL_SHADER_TYPE_SIZE.get(type),
				                  }
		                      ));

		if (count != 1) {
			// TODO support them
			log::log(MSG(warn) << "Found array uniform " << name.data() << " in shader. Arrays are unsupported.");
		}

		if (type == GL_SAMPLER_2D) {
			if (tex_unit >= caps.max_texture_slots) {
				throw Error(MSG(err)
				            << "Tried to create shader that uses more texture sampler uniforms "
				            << "than there are texture unit slots available.");
			}
			this->texunits_per_unifs.insert(std::make_pair(name.data(), tex_unit));
			tex_unit += 1;
		}

		// TODO optimized away detection
		if (0 == -1) {
			log::log(MSG(warn)
			         << "OpenGL shader uniform " << name.data() << " was present in the source, but isn't present in the program. Probably optimized away.");
			continue;
		}
	}

	for (GLuint i_attrib = 0; i_attrib < attrib_count; ++i_attrib) {
		GLint size;
		GLenum type;
		glGetActiveAttrib(
			handle,
			i_attrib,
			name.size(),
			0,
			&size,
			&type,
			name.data()
		);

		this->attribs.insert(std::make_pair(
			                     name.data(),
			                     GlVertexAttrib {
				                     type,
				                     GLint(i_attrib),
				                     size,
			                     }
		                     ));
	}

	log::log(MSG(info) << "Created OpenGL shader program");

	log::log(MSG(dbg) << "Uniforms: ");
	for (auto const &pair : this->uniforms) {
		log::log(MSG(dbg) << "(" << pair.second.location << ") " << pair.first << ": " << pair.second.type);
	}
	log::log(MSG(dbg) << "Vertex attributes: ");
	for (auto const &pair : this->attribs) {
		log::log(MSG(dbg) << "(" << pair.second.location << ") " << pair.first << ": " << pair.second.type);
	}
}

void GlShaderProgram::use() const {
	glUseProgram(*this->handle);

	for (auto const &pair : this->textures_per_texunits) {
		// We have to bind the texture to their texture units here because
		// the texture unit bindings are global to the context. Each time
		// the shader switches, it is possible that some other shader overwrote
		// these, and since we want the uniform values to persist across execute_with
		// calls, we have to set them more often than just on execute_with.
		glActiveTexture(GL_TEXTURE0 + pair.first);
		glBindTexture(GL_TEXTURE_2D, pair.second);

		// By the time we call bind, the texture may have been deleted, but if it's fixed
		// afterwards using execute_with, the render state will still be fine, so we can ignore
		// this error.
		// TODO this will swallow actual errors elsewhere, and should be avoided. how?
		glGetError();
	}
}

void GlShaderProgram::execute_with(const GlUniformInput *unif_in, const GlGeometry *geom) {
	ENSURE(unif_in->program == this);

	this->use();

	uint8_t const* data = unif_in->update_data.data();
	for (auto const &pair : unif_in->update_offs) {
		uint8_t const* ptr = data + pair.second;
		auto loc = this->uniforms[pair.first].location;

		switch (this->uniforms[pair.first].type) {
		case GL_INT:
			glUniform1i(loc, *reinterpret_cast<const GLint*>(ptr));
			break;
		case GL_UNSIGNED_INT:
			glUniform1ui(loc, *reinterpret_cast<const GLuint*>(ptr));
			break;
		case GL_FLOAT:
			glUniform1f(loc, *reinterpret_cast<const float*>(ptr));
			break;
		case GL_DOUBLE:
			// TODO requires an extension
			glUniform1d(loc, *reinterpret_cast<const double*>(ptr));
			break;
		case GL_FLOAT_VEC2:
			glUniform2fv(loc, 1, reinterpret_cast<const float*>(ptr));
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv(loc, 1, reinterpret_cast<const float*>(ptr));
			break;
		case GL_FLOAT_VEC4:
			glUniform4fv(loc, 1, reinterpret_cast<const float*>(ptr));
			break;
		case GL_FLOAT_MAT3:
			glUniformMatrix3fv(loc, 1, false, reinterpret_cast<const float*>(ptr));
			break;
		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(loc, 1, false, reinterpret_cast<const float*>(ptr));
			break;
		case GL_INT_VEC2:
			glUniform2iv(loc, 1, reinterpret_cast<const GLint*>(ptr));
			break;
		case GL_INT_VEC3:
			glUniform3iv(loc, 1, reinterpret_cast<const GLint*>(ptr));
			break;
		case GL_SAMPLER_2D: {
			GLuint tex_unit = this->texunits_per_unifs[pair.first];
			GLuint tex = *reinterpret_cast<const GLuint*>(ptr);
			glActiveTexture(GL_TEXTURE0 + tex_unit);
			glBindTexture(GL_TEXTURE_2D, tex);
			// TODO: maybe call this at a more appropriate position
			glUniform1i(loc, tex_unit);
			this->textures_per_texunits[tex_unit] = tex;
			break;
		}
		default:
			throw Error(MSG(err) << "Tried to upload unknown uniform type to GL shader.");
		}
	}

	if (geom != nullptr) {
		// TODO read obj.blend + family
		geom->draw();
	}
}

std::map<size_t, resources::vertex_input_t> GlShaderProgram::vertex_attributes() const {
	std::map<size_t, resources::vertex_input_t> attrib_map;

	for (auto const& attr : this->attribs) {
		attrib_map[attr.second.location] = GL_VERT_IN_TYPE.get(attr.second.type);
	}

	return attrib_map;
}

std::unique_ptr<UniformInput> GlShaderProgram::new_unif_in() {
	auto in = std::make_unique<GlUniformInput>();
	in->program = this;
	return in;
}

bool GlShaderProgram::has_uniform(const char* name) {
	return this->uniforms.count(name) == 1;
}

void GlShaderProgram::set_unif(UniformInput *in, const char *unif, void const* val, GLenum type) {
	GlUniformInput *unif_in = static_cast<GlUniformInput*>(in);

	if (unlikely(this->uniforms.count(unif) == 0)) {
		throw Error(MSG(err) << "Tried to set uniform " << unif << " that does not exist in the shader program.");
	}

	auto const& unif_data = this->uniforms.at(unif);

	if (unlikely(type != unif_data.type)) {
		throw Error(MSG(err) << "Tried to set uniform " << unif << " to a value of the wrong type.");
	}

	size_t size = GL_SHADER_TYPE_SIZE.get(unif_data.type);

	if (unif_in->update_offs.count(unif) == 1) {
		// already wrote to this uniform since last upload
		size_t off = unif_in->update_offs[unif];
		memcpy(unif_in->update_data.data() + off, val, size);
	} else {
		// first time writing to this uniform since last upload
		size_t prev_size = unif_in->update_data.size();
		unif_in->update_data.resize(prev_size + size);
		memcpy(unif_in->update_data.data() + prev_size, val, size);
		unif_in->update_offs.emplace(unif, prev_size);
	}
}

void GlShaderProgram::set_i32(UniformInput *in, const char *unif, int32_t val) {
	this->set_unif(in, unif, &val, GL_INT);
}

void GlShaderProgram::set_u32(UniformInput *in, const char *unif, uint32_t val) {
	this->set_unif(in, unif, &val, GL_UNSIGNED_INT);
}

void GlShaderProgram::set_f32(UniformInput *in, const char *unif, float val) {
	this->set_unif(in, unif, &val, GL_FLOAT);
}

void GlShaderProgram::set_f64(UniformInput *in, const char *unif, double val) {
	// TODO requires extension
	this->set_unif(in, unif, &val, GL_DOUBLE);
}

void GlShaderProgram::set_v2f32(UniformInput *in, const char *unif, Eigen::Vector2f const& val) {
	this->set_unif(in, unif, &val, GL_FLOAT_VEC2);
}

void GlShaderProgram::set_v3f32(UniformInput *in, const char *unif, Eigen::Vector3f const& val) {
	this->set_unif(in, unif, &val, GL_FLOAT_VEC3);
}

void GlShaderProgram::set_v4f32(UniformInput *in, const char *unif, Eigen::Vector4f const& val) {
	this->set_unif(in, unif, &val, GL_FLOAT_VEC4);
}

void GlShaderProgram::set_m4f32(UniformInput *in, const char *unif, Eigen::Matrix4f const& val) {
	this->set_unif(in, unif, val.data(), GL_FLOAT_MAT4);
}

void GlShaderProgram::set_tex(UniformInput *in, const char *unif, Texture2d const* val) {
	auto const& tex = *static_cast<const GlTexture2d*>(val);
	GLuint handle = tex.get_handle();
	this->set_unif(in, unif, &handle, GL_SAMPLER_2D);
}

}}} // openage::renderer::opengl
