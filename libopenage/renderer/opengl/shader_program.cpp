// Copyright 2013-2018 the openage authors. See copying.md for legal info.

#include "shader_program.h"

#include <algorithm>
#include <unordered_set>

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
		glGetProgramInfoLog(program, loglen, nullptr, infolog.data());

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
	: GlSimpleObject([] (GLuint handle) { glDeleteProgram(handle); } )
	, validated(false) {
	GLuint handle = glCreateProgram();
	this->handle = handle;

	std::vector<GlShader> shaders;
	for (auto const& src : srcs) {
		GlShader shader(src);
		glAttachShader(handle, shader.get_handle());
		shaders.push_back(std::move(shader));
	}

	glLinkProgram(handle);
	check_program_status(handle, GL_LINK_STATUS);

	// after linking we can delete the shaders
	for (auto const& shdr : shaders) {
		glDetachShader(handle, shdr.get_handle());
	}

	// query program information
	GLint val;
	glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &val);
	size_t attrib_count = val;
	glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &val);
	size_t max_name_len = val;
	glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &val);
	size_t unif_count = val;
	glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &val);
	max_name_len = std::max(size_t(val), max_name_len);
	glGetProgramiv(handle, GL_ACTIVE_UNIFORM_BLOCKS, &val);
	size_t unif_block_count = val;
	glGetProgramiv(handle, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &val);
	max_name_len = std::max(size_t(val), max_name_len);

	std::vector<char> name(max_name_len);
	// Indices of uniforms within named blocks.
	std::unordered_set<GLuint> in_block_unifs;

	GLuint block_binding = 0;

	// Extract uniform block descriptions.
	for (GLuint i_unif_block = 0; i_unif_block < unif_block_count; ++i_unif_block) {
		glGetActiveUniformBlockName(
			handle,
			i_unif_block,
			name.size(),
			nullptr,
			name.data()
		);

		std::string block_name(name.data());

		GLint data_size;
		glGetActiveUniformBlockiv(handle, i_unif_block, GL_UNIFORM_BLOCK_DATA_SIZE, &data_size);

		glGetActiveUniformBlockiv(handle, i_unif_block, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &val);
		std::vector<GLint> uniform_indices(val);
		glGetActiveUniformBlockiv(handle, i_unif_block, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniform_indices.data());

		std::unordered_map<std::string, GlInBlockUniform> uniforms;
		for (GLuint const i_unif : uniform_indices) {
			in_block_unifs.insert(i_unif);

			GLenum type;
			GLint offset, count, stride;

			glGetActiveUniform(
				handle,
				i_unif,
				name.size(),
				nullptr,
				&count,
				&type,
				name.data()
			);

			glGetActiveUniformsiv(handle, 1, &i_unif, GL_UNIFORM_OFFSET, &offset);
			glGetActiveUniformsiv(handle, 1, &i_unif, GL_UNIFORM_ARRAY_STRIDE, &stride);
			if (stride == 0) {
				// The uniform is not an array, but it's declared in a named block and hence might
				// be a matrix whose stride we need to know.
				glGetActiveUniformsiv(handle, 1, &i_unif, GL_UNIFORM_MATRIX_STRIDE, &stride);
			}

			// We do not need to handle sampler types here like in the uniform loop below,
			// because named blocks cannot contain samplers.

			uniforms.insert(std::make_pair(
				name.data(),
				GlInBlockUniform {
					type,
					size_t(offset),
					size_t(count) * GL_SHADER_TYPE_SIZE.get(type),
					size_t(stride),
					size_t(count)
				}
			));
		}

		ENSURE(block_binding < caps.max_uniform_buffer_bindings,
			"Tried to create an OpenGL shader that uses more uniform blocks "
			<< "than there are binding points (" << caps.max_uniform_buffer_bindings
			<< " available)."
		);

		glUniformBlockBinding(handle, i_unif_block, block_binding);

		this->uniform_blocks.insert(std::make_pair(
			block_name,
			GlUniformBlock {
				i_unif_block,
				size_t(data_size),
				std::move(uniforms),
				block_binding
			}
		));

		block_binding += 1;
	}

	GLuint tex_unit = 0;

	// Extract information about uniforms in the default block.
	for (GLuint i_unif = 0; i_unif < unif_count; ++i_unif) {
		if (in_block_unifs.count(i_unif) == 1) {
			// Skip uniforms within named blocks.
			continue;
		}

		GLint count;
		GLenum type;
		glGetActiveUniform(
			handle,
			i_unif,
			name.size(),
			nullptr,
			&count,
			&type,
			name.data()
		);

		GLuint loc = glGetUniformLocation(handle, name.data());

		this->uniforms.insert(std::make_pair(
			name.data(),
			GlUniform {
				type,
				loc
			}
		));

		if (type == GL_SAMPLER_2D) {
			ENSURE(tex_unit < caps.max_texture_slots,
				"Tried to create an OpenGL shader that uses more texture sampler uniforms "
				<< "than there are texture unit slots (" << caps.max_texture_slots << " available)."
			);

			this->texunits_per_unifs.insert(std::make_pair(name.data(), tex_unit));

			tex_unit += 1;
		}
	}

	// Extract vertex attribute descriptions.
	for (GLuint i_attrib = 0; i_attrib < attrib_count; ++i_attrib) {
		GLint size;
		GLenum type;
		glGetActiveAttrib(
			handle,
			i_attrib,
			name.size(),
			nullptr,
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

	if (!this->uniform_blocks.empty()) {
		log::log(MSG(dbg) << "Uniform blocks: ");
		for (auto const& pair : this->uniform_blocks) {
			log::log(MSG(dbg) << "(" << pair.second.index << ") " << pair.first
			                  << " (size: " << pair.second.data_size << ") {");
			for (auto const& unif_pair : pair.second.uniforms) {
				log::log(MSG(dbg) << "\t+" << unif_pair.second.offset
				                  << " " << unif_pair.first << ": "
				                  << GLSL_TYPE_NAME.get(unif_pair.second.type));
			}
			log::log(MSG(dbg) << "}");
		}
	}

	if (!this->uniforms.empty()) {
		log::log(MSG(dbg) << "Uniforms: ");
		for (auto const& pair : this->uniforms) {
			log::log(MSG(dbg) << "(" << pair.second.location << ") " << pair.first << ": "
			                  << GLSL_TYPE_NAME.get(pair.second.type));
		}
	}

	if (!this->attribs.empty()) {
		log::log(MSG(dbg) << "Vertex attributes: ");
		for (auto const& pair : this->attribs) {
			log::log(MSG(dbg) << "(" << pair.second.location << ") " << pair.first << ": "
			                  << GLSL_TYPE_NAME.get(pair.second.type));
		}
	}
}

void GlShaderProgram::use() {
	if (!this->validated) {
		// TODO(Vtec234): validation depends on the context state, so this might be worth calling
		// more than once. However, once per frame is probably too much.
		glValidateProgram(*this->handle);
		check_program_status(*this->handle, GL_VALIDATE_STATUS);

		this->validated = true;
	}

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
	ENSURE(unif_in->program == this, "Uniform input passed to different shader than it was created with.");

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
			glUniformMatrix3fv(loc, 1, GLboolean(false), reinterpret_cast<const float*>(ptr));
			break;
		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(loc, 1, GLboolean(false), reinterpret_cast<const float*>(ptr));
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
	auto *unif_in = static_cast<GlUniformInput*>(in);

	ENSURE(this->uniforms.count(unif) != 0,
		"Tried to set uniform " << unif << " that does not exist in the shader program."
	);

	auto const& unif_data = this->uniforms.at(unif);

	ENSURE(type == unif_data.type,
		"Tried to set uniform " << unif << " to a value of the wrong type."
	);

	size_t size = GL_SHADER_TYPE_SIZE.get(unif_data.type);

	if (unif_in->update_offs.count(unif) == 1) {
		// already wrote to this uniform since last upload
		size_t off = unif_in->update_offs[unif];
		memcpy(unif_in->update_data.data() + off, val, size);
	} else {
		// first time writing to this uniform since last upload, so
		// extend the buffer before storing the uniform value
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
