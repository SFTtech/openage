// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "shader_program.h"

#include "../../error/error.h"
#include "../../log/log.h"
#include "../../datastructure/constexpr_map.h"

#include "texture.h"
#include "shader.h"
#include "geometry.h"


namespace openage {
namespace renderer {
namespace opengl {

static constexpr auto glsl_to_gl_type = datastructure::create_const_map<const char*, GLenum>(
	std::make_pair("int", GL_INT),
	std::make_pair("uint", GL_UNSIGNED_INT),
	std::make_pair("float", GL_FLOAT),
	std::make_pair("double", GL_DOUBLE),
	std::make_pair("vec2", GL_FLOAT_VEC2),
	std::make_pair("vec3", GL_FLOAT_VEC3),
	std::make_pair("mat3", GL_FLOAT_MAT3),
	std::make_pair("mat4", GL_FLOAT_MAT4),
	std::make_pair("ivec2", GL_INT_VEC2),
	std::make_pair("ivec3", GL_INT_VEC3),
	std::make_pair("sampler2D", GL_SAMPLER_2D)
);

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

static constexpr auto gl_type_size = datastructure::create_const_map<GLenum, size_t>(
	std::make_pair(GL_FLOAT, 4),
	std::make_pair(GL_FLOAT_VEC2, 8),
	std::make_pair(GL_FLOAT_VEC3, 12),
	std::make_pair(GL_FLOAT_VEC4, 16),
	std::make_pair(GL_INT, 4),
	std::make_pair(GL_INT_VEC2, 8),
	std::make_pair(GL_INT_VEC3, 12),
	std::make_pair(GL_INT_VEC4, 16),
	std::make_pair(GL_UNSIGNED_INT, 4),
	std::make_pair(GL_UNSIGNED_INT_VEC2, 8),
	std::make_pair(GL_UNSIGNED_INT_VEC3, 12),
	std::make_pair(GL_UNSIGNED_INT_VEC4, 16),
	std::make_pair(GL_BOOL, 1),
	std::make_pair(GL_BOOL_VEC2, 2),
	std::make_pair(GL_BOOL_VEC3, 3),
	std::make_pair(GL_BOOL_VEC4, 4),
	std::make_pair(GL_FLOAT_MAT2, 16),
	std::make_pair(GL_FLOAT_MAT3, 36),
	std::make_pair(GL_FLOAT_MAT4, 64),
	std::make_pair(GL_SAMPLER_1D, 4),
	std::make_pair(GL_SAMPLER_2D, 4),
	std::make_pair(GL_SAMPLER_3D, 4),
	std::make_pair(GL_SAMPLER_CUBE, 4)
);

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
				                      size_t(count) * gl_type_size.get(type),
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

}

void GlShaderProgram::execute_with(const GlUniformInput *unif_in, const GlGeometry *geom) {
	assert(unif_in->program == this);

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
			//log::log(INFO << "tex_unit   "<<tex_unit<<"   tex  "<< tex);
			// TODO: maybe call this at a more appropriate position
			glUniform1i(loc, tex_unit);
			//log::log(INFO << pair.first<<"   "<<loc<<" value  "<<tex_unit);
			//this->textures_per_texunits[tex_unit] = tex;
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


void GlShaderProgram::send_uniform(const GlUniformInput *unif_in) {
	assert(unif_in->program == this);

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

			//glUniform1i(loc, pair.first);
			break;
		}
		default:
			throw Error(MSG(err) << "Tried to upload unknown uniform type to GL shader.");
		}
	}

}

std::shared_ptr<UniformInput> GlShaderProgram::new_unif_in() {
	auto in = std::make_shared<GlUniformInput>();
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

	size_t size = gl_type_size.get(unif_data.type);

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

void GlShaderProgram::set_tex(UniformInput *in, const char *unif, Texture const* val) {
	auto const& tex = *static_cast<const GlTexture*>(val);
	GLuint handle = tex.get_handle();
	this->set_unif(in, unif, &handle, GL_SAMPLER_2D);
}

int GlShaderProgram::texture_array(int num){
	std::string temp = "texture_";
	for(int i = 0;i<num;i++){
		std::string temp_2 = temp + std::to_string(i); 
		auto loc = this->uniforms[temp_2].location;
		glUniform1i(loc,i);
		//log::log(INFO << temp_2<<"   "<<loc<<" value  "<< i);
	}
	return 0;
}

}}} // openage::renderer::opengl
