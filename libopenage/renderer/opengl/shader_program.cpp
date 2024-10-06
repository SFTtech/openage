// Copyright 2013-2024 the openage authors. See copying.md for legal info.

#include "shader_program.h"

#include <algorithm>
#include <cstdio>
#include <unordered_set>

#include "datastructure/constexpr_map.h"
#include "error/error.h"
#include "log/log.h"

#include "renderer/opengl/context.h"
#include "renderer/opengl/error.h"
#include "renderer/opengl/geometry.h"
#include "renderer/opengl/lookup.h"
#include "renderer/opengl/shader.h"
#include "renderer/opengl/texture.h"
#include "renderer/opengl/uniform_buffer.h"
#include "renderer/opengl/uniform_input.h"
#include "renderer/opengl/util.h"


namespace openage::renderer::opengl {

static void check_program_status(GLuint program, GLenum what_to_check) {
	GLint status = GL_FALSE;
	glGetProgramiv(program, what_to_check, &status);

	GlContext::check_error();

	if (status != GL_TRUE) {
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

		GLint loglen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglen);

		std::vector<char> infolog(loglen);
		GLint real_loglen;
		glGetProgramInfoLog(program, loglen, &real_loglen, infolog.data());

		std::string gl_message = "no reason returned";
		if (infolog.size() and strlen(infolog.data())) {
			gl_message = infolog.data();
		}

		throw Error(ERR << "OpenGL shader program " << what_str << " failed (len " << loglen << ", real: " << real_loglen << "):\n"
		                << gl_message,
		            true);
	}
}

GlShaderProgram::GlShaderProgram(const std::shared_ptr<GlContext> &context,
                                 const std::vector<resources::ShaderSource> &srcs) :
	GlSimpleObject(context,
                   [](GLuint handle) { glDeleteProgram(handle); }),
	validated(false) {
	const gl_context_spec &caps = context->get_specs();

	GLuint handle = glCreateProgram();
	this->handle = handle;

	std::vector<GlShader> shaders;
	for (auto const &src : srcs) {
		GlShader shader{context, src};
		glAttachShader(handle, shader.get_handle());
		shaders.push_back(std::move(shader));
	}

	glLinkProgram(handle);
	check_program_status(handle, GL_LINK_STATUS);

	// after linking we can delete the shaders
	for (auto const &shdr : shaders) {
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
			name.data());

		std::string block_name(name.data());

		GLint data_size;
		glGetActiveUniformBlockiv(handle, i_unif_block, GL_UNIFORM_BLOCK_DATA_SIZE, &data_size);

		glGetActiveUniformBlockiv(handle, i_unif_block, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &val);
		std::vector<GLint> uniform_indices(val);
		glGetActiveUniformBlockiv(handle, i_unif_block, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniform_indices.data());

		std::vector<GlInBlockUniform> uniforms;
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
				name.data());

			glGetActiveUniformsiv(handle, 1, &i_unif, GL_UNIFORM_OFFSET, &offset);
			glGetActiveUniformsiv(handle, 1, &i_unif, GL_UNIFORM_ARRAY_STRIDE, &stride);
			if (stride == 0) {
				// The uniform is not an array, but it's declared in a named block and hence might
				// be a matrix whose stride we need to know.
				glGetActiveUniformsiv(handle, 1, &i_unif, GL_UNIFORM_MATRIX_STRIDE, &stride);
			}

			// We do not need to handle sampler types here like in the uniform loop below,
			// because named blocks cannot contain samplers.

			uniforms.push_back(
				GlInBlockUniform{
					type,
					size_t(offset),
					size_t(count) * GL_UNIFORM_TYPE_SIZE.get(type),
					size_t(stride),
					size_t(count),
					std::string(name.data())});
		}

		// ENSURE(block_binding < caps.max_uniform_buffer_bindings,
		//        "Tried to create an OpenGL shader that uses more uniform blocks "
		//            << "than there are binding points (" << caps.max_uniform_buffer_bindings
		//            << " available).");
		//
		// glUniformBlockBinding(handle, i_unif_block, block_binding);
		// block_binding += 1;

		this->uniform_blocks.insert(std::make_pair(
			block_name,
			GlUniformBlock{
				i_unif_block,
				size_t(data_size),
				std::move(uniforms),
				block_binding}));
	}

	GLuint tex_unit_id = 0;

	// Extract information about uniforms in the default block.

	// the uniform ID is the index in the uniforms vector
	uniform_id_t unif_id = 0;
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
			name.data());

		GLuint loc = glGetUniformLocation(handle, name.data());

		this->uniforms.push_back({type, loc, std::nullopt});

		this->uniforms_by_name.insert(std::make_pair(
			name.data(),
			unif_id));

		if (type == GL_SAMPLER_2D) {
			ENSURE(tex_unit_id < caps.max_texture_slots,
			       "Tried to create an OpenGL shader that uses more texture sampler uniforms "
			           << "than there are texture unit slots (" << caps.max_texture_slots << " available).");

			this->uniforms[unif_id].tex_unit = tex_unit_id;

			tex_unit_id += 1;
		}

		// Increment uniform ID
		unif_id += 1;
	}

	// Resize the texture unit bindings
	// to number of texture units used by the shader
	this->textures_per_texunits.resize(tex_unit_id);

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
			name.data());

		this->attribs.insert(std::make_pair(
			name.data(),
			GlVertexAttrib{
				type,
				GLint(i_attrib),
				size,
			}));
	}

	log::log(MSG(info) << "Created OpenGL shader program");

	if (!this->uniform_blocks.empty()) {
		log::log(MSG(dbg) << "Uniform blocks: ");
		for (auto const &pair : this->uniform_blocks) {
			log::log(MSG(dbg) << "(" << pair.second.index << ") " << pair.first
			                  << " (size: " << pair.second.data_size << ") {");
			for (auto const &unif : pair.second.uniforms) {
				log::log(MSG(dbg) << "\t+" << unif.offset
				                  << " " << unif.name << ": "
				                  << GLSL_TYPE_NAME.get(unif.type));
			}
			log::log(MSG(dbg) << "}");
		}
	}

	if (!this->uniforms.empty()) {
		log::log(MSG(dbg) << "Uniforms: ");
		for (const auto &pair : this->uniforms_by_name) {
			const auto &unif_info = this->uniforms[pair.second];
			log::log(MSG(dbg) << "(" << unif_info.location << ") "
			                  << pair.first << ": "
			                  << GLSL_TYPE_NAME.get(unif_info.type));
		}
	}

	if (!this->attribs.empty()) {
		log::log(MSG(dbg) << "Vertex attributes: ");
		for (auto const &pair : this->attribs) {
			log::log(MSG(dbg) << "(" << pair.second.location << ") " << pair.first << ": "
			                  << GLSL_TYPE_NAME.get(pair.second.type));
		}
	}
}


void GlShaderProgram::use() {
	// the setup doesn't need to be done if this program is already active.
	if (this->in_use()) {
		return;
	}

	if (!this->validated) {
		// TODO(Vtec234): validation depends on the context state, so this might be worth calling
		// more than once. However, once per frame is probably too much.
		glValidateProgram(*this->handle);
		check_program_status(*this->handle, GL_VALIDATE_STATUS);

		this->validated = true;
	}

	glUseProgram(*this->handle);

	// store this program as the active one
	this->context->set_current_program(
		std::static_pointer_cast<GlShaderProgram>(
			this->shared_from_this()));

	for (size_t i = 0; i < this->textures_per_texunits.size(); ++i) {
		auto &tex_unit = this->textures_per_texunits[i];
		if (not tex_unit) {
			continue;
		}

		if (not glIsTexture(*tex_unit)) {
			// By the time we use the shader again, the texture may have been deleted
			// but if it's fixed afterwards using update_uniforms, the render state
			// will still be fine
			// We can free the texture unit in this case
			tex_unit = std::nullopt;
			continue;
		}

		// We have to bind the texture to their texture units here because
		// the texture unit bindings are global to the context. Each time
		// the shader switches, it is possible that some other shader overwrote
		// these, and since we want the uniform values to persist across update_uniforms
		// calls, we have to set them more often than just on update_uniforms.
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, *tex_unit);
	}
}


bool GlShaderProgram::in_use() const {
	return this->context->get_current_program().lock().get() == this;
}


void GlShaderProgram::update_uniforms(std::shared_ptr<GlUniformInput> const &unif_in) {
	ENSURE(unif_in->get_program().get() == this, "Uniform input passed to different shader than it was created with.");

	// TODO: use glProgramUniform when we're on OpenGL 4.1
	// then we don't need to "use" and then call glUniform*

	if (not this->in_use()) {
		this->use();
	}

	const auto &update_offs = unif_in->update_offs;
	const auto &used_uniforms = unif_in->used_uniforms;
	const auto &uniforms = this->uniforms;
	uint8_t const *data = unif_in->update_data.data();

	size_t unif_count = used_uniforms.size();
	for (size_t i = 0; i < unif_count; ++i) {
		uniform_id_t unif_id = used_uniforms[i];

		const auto &update_off = update_offs[unif_id];
		uint8_t const *ptr = data + update_off.offset;

		const auto &unif = uniforms[unif_id];
		auto loc = unif.location;

		switch (unif.type) {
		case GL_INT:
			glUniform1i(loc, *reinterpret_cast<const GLint *>(ptr));
			break;
		case GL_UNSIGNED_INT:
			glUniform1ui(loc, *reinterpret_cast<const GLuint *>(ptr));
			break;
		case GL_FLOAT:
			glUniform1f(loc, *reinterpret_cast<const float *>(ptr));
			break;
		case GL_DOUBLE:
			// TODO requires an extension
			glUniform1d(loc, *reinterpret_cast<const double *>(ptr));
			break;
		case GL_BOOL:
			glUniform1ui(loc, *reinterpret_cast<const bool *>(ptr));
			break;
		case GL_FLOAT_VEC2:
			glUniform2fv(loc, 1, reinterpret_cast<const float *>(ptr));
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv(loc, 1, reinterpret_cast<const float *>(ptr));
			break;
		case GL_FLOAT_VEC4:
			glUniform4fv(loc, 1, reinterpret_cast<const float *>(ptr));
			break;
		case GL_INT_VEC2:
			glUniform2iv(loc, 1, reinterpret_cast<const GLint *>(ptr));
			break;
		case GL_INT_VEC3:
			glUniform3iv(loc, 1, reinterpret_cast<const GLint *>(ptr));
			break;
		case GL_INT_VEC4:
			glUniform4iv(loc, 1, reinterpret_cast<const GLint *>(ptr));
			break;
		case GL_UNSIGNED_INT_VEC2:
			glUniform2uiv(loc, 1, reinterpret_cast<const GLuint *>(ptr));
			break;
		case GL_UNSIGNED_INT_VEC3:
			glUniform3uiv(loc, 1, reinterpret_cast<const GLuint *>(ptr));
			break;
		case GL_UNSIGNED_INT_VEC4:
			glUniform4uiv(loc, 1, reinterpret_cast<const GLuint *>(ptr));
			break;
		case GL_FLOAT_MAT3:
			glUniformMatrix3fv(loc, 1, GLboolean(false), reinterpret_cast<const float *>(ptr));
			break;
		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(loc, 1, GLboolean(false), reinterpret_cast<const float *>(ptr));
			break;
		case GL_SAMPLER_2D: {
			ENSURE(unif.tex_unit,
			       "Tried to access texture unit for uniform that has no texture unit assigned.");
			GLuint tex_unit_id = *unif.tex_unit;

			GLuint tex = *reinterpret_cast<const GLuint *>(ptr);
			glActiveTexture(GL_TEXTURE0 + tex_unit_id);
			glBindTexture(GL_TEXTURE_2D, tex);
			// TODO: maybe call this at a more appropriate position
			glUniform1i(loc, tex_unit_id);
			ENSURE(tex_unit_id < this->textures_per_texunits.size(),
			       "Tried to assign texture to non-existant texture unit at index "
			           << tex_unit_id
			           << " (max: " << this->textures_per_texunits.size() << ").");
			this->textures_per_texunits[tex_unit_id] = tex;
			break;
		}
		default:
			throw Error(MSG(err) << "Tried to upload unknown uniform type to GL shader.");
		}
	}
}

const GlUniformBlock &GlShaderProgram::get_uniform_block(const char *name) const {
	return this->uniform_blocks.at(name);
}

std::map<size_t, resources::vertex_input_t> GlShaderProgram::vertex_attributes() const {
	std::map<size_t, resources::vertex_input_t> attrib_map;

	for (auto const &attr : this->attribs) {
		attrib_map[attr.second.location] = GL_VERT_IN_TYPE.get(attr.second.type);
	}

	return attrib_map;
}

std::shared_ptr<UniformInput> GlShaderProgram::new_unif_in() {
	auto in = std::make_shared<GlUniformInput>(this->shared_from_this());

	return in;
}

uniform_id_t GlShaderProgram::get_uniform_id(const char *name) {
	return this->uniforms_by_name.at(name);
}

const std::vector<GlUniform> &GlShaderProgram::get_uniforms() const {
	return this->uniforms;
}

const std::unordered_map<std::string, GlUniformBlock> &GlShaderProgram::get_uniform_blocks() const {
	return this->uniform_blocks;
}

bool GlShaderProgram::has_uniform(const char *name) {
	return this->uniforms_by_name.contains(name);
}

void GlShaderProgram::bind_uniform_buffer(const char *block_name, std::shared_ptr<UniformBuffer> const &buffer) {
	ENSURE(this->uniform_blocks.count(block_name) == 1,
	       "Tried to set binding point for uniform block " << block_name << " that does not exist in the shader program.");

	auto gl_buffer = std::dynamic_pointer_cast<GlUniformBuffer>(buffer);
	auto &block = this->uniform_blocks[block_name];

	// Check if the uniform buffer matches the block definition
	for (auto const &unif : block.uniforms) {
		ENSURE(gl_buffer->has_uniform(unif.name.c_str()),
		       "Uniform buffer does not contain uniform '" << unif.name << "' required by block " << block_name);
	}

	block.binding_point = gl_buffer->get_binding_point();
	glUniformBlockBinding(*this->handle, block.index, block.binding_point);
}

void GlShaderProgram::set_unif(UniformInput &in,
                               const char *unif,
                               void const *val,
                               size_t size,
                               GLenum type) {
	auto uniform_id = this->uniforms_by_name.find(unif);
	ENSURE(uniform_id != std::end(this->uniforms_by_name),
	       "Tried to set uniform '" << unif << "' that does not exist in the shader program.");

	this->set_unif(in, uniform_id->second, val, size, type);
}

void GlShaderProgram::set_unif(UniformInput &in,
                               uniform_id_t unif_id,
                               void const *val,
                               size_t size,
                               GLenum type) {
	auto &unif_in = dynamic_cast<GlUniformInput &>(in);

	ENSURE(unif_id < this->uniforms.size(),
	       "Tried to set uniform '" << unif_id << "' that does not exist in the shader program.");

	ENSURE(unif_id < this->uniforms.size(),
	       "Tried to set uniform with invalid ID " << unif_id);

	auto const &unif_info = this->uniforms[unif_id];
	ENSURE(type == unif_info.type,
	       "Tried to set uniform '" << unif_id << "' to a value of the wrong type.");

	auto &update_off = unif_in.update_offs[unif_id];
	auto offset = update_off.offset;
	memcpy(unif_in.update_data.data() + offset, val, size);
	if (not update_off.used) [[unlikely]] { // only true if the uniform value was not set before
		auto lower_bound = std::lower_bound(
			std::begin(unif_in.used_uniforms),
			std::end(unif_in.used_uniforms),
			unif_id);
		unif_in.used_uniforms.insert(lower_bound, unif_id);
		update_off.used = true;
	}
}

void GlShaderProgram::set_i32(UniformInput &in, const char *unif, int32_t val) {
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_INT), GL_INT);
}

void GlShaderProgram::set_u32(UniformInput &in, const char *unif, uint32_t val) {
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_UNSIGNED_INT), GL_UNSIGNED_INT);
}

void GlShaderProgram::set_f32(UniformInput &in, const char *unif, float val) {
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_FLOAT), GL_FLOAT);
}

void GlShaderProgram::set_f64(UniformInput &in, const char *unif, double val) {
	// TODO requires extension
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_DOUBLE), GL_DOUBLE);
}

void GlShaderProgram::set_bool(UniformInput &in, const char *unif, bool val) {
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_BOOL), GL_BOOL);
}

void GlShaderProgram::set_v2f32(UniformInput &in, const char *unif, Eigen::Vector2f const &val) {
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_FLOAT_VEC2), GL_FLOAT_VEC2);
}

void GlShaderProgram::set_v3f32(UniformInput &in, const char *unif, Eigen::Vector3f const &val) {
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_FLOAT_VEC3), GL_FLOAT_VEC3);
}

void GlShaderProgram::set_v4f32(UniformInput &in, const char *unif, Eigen::Vector4f const &val) {
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_FLOAT_VEC4), GL_FLOAT_VEC4);
}

void GlShaderProgram::set_v2i32(UniformInput &in, const char *unif, Eigen::Vector2i const &val) {
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_INT_VEC2), GL_INT_VEC2);
}

void GlShaderProgram::set_v3i32(UniformInput &in, const char *unif, Eigen::Vector3i const &val) {
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_INT_VEC3), GL_INT_VEC3);
}

void GlShaderProgram::set_v4i32(UniformInput &in, const char *unif, Eigen::Vector4i const &val) {
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_INT_VEC4), GL_INT_VEC4);
}

void GlShaderProgram::set_v2ui32(UniformInput &in, const char *unif, Eigen::Vector2<uint32_t> const &val) {
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_UNSIGNED_INT_VEC2), GL_UNSIGNED_INT_VEC2);
}

void GlShaderProgram::set_v3ui32(UniformInput &in, const char *unif, Eigen::Vector3<uint32_t> const &val) {
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_UNSIGNED_INT_VEC3), GL_UNSIGNED_INT_VEC3);
}

void GlShaderProgram::set_v4ui32(UniformInput &in, const char *unif, Eigen::Vector4<uint32_t> const &val) {
	this->set_unif(in, unif, &val, get_uniform_type_size(GL_UNSIGNED_INT_VEC4), GL_UNSIGNED_INT_VEC4);
}

void GlShaderProgram::set_m4f32(UniformInput &in, const char *unif, Eigen::Matrix4f const &val) {
	this->set_unif(in, unif, val.data(), get_uniform_type_size(GL_FLOAT_MAT4), GL_FLOAT_MAT4);
}

void GlShaderProgram::set_tex(UniformInput &in, const char *unif, std::shared_ptr<Texture2d> const &val) {
	auto tex = std::dynamic_pointer_cast<GlTexture2d>(val);
	GLuint handle = tex->get_handle();
	this->set_unif(in, unif, &handle, get_uniform_type_size(GL_SAMPLER_2D), GL_SAMPLER_2D);
}

void GlShaderProgram::set_i32(UniformInput &in, uniform_id_t id, int32_t val) {
	this->set_unif(in, id, &val, get_uniform_type_size(GL_INT), GL_INT);
}

void GlShaderProgram::set_u32(UniformInput &in, uniform_id_t id, uint32_t val) {
	this->set_unif(in, id, &val, get_uniform_type_size(GL_UNSIGNED_INT), GL_UNSIGNED_INT);
}

void GlShaderProgram::set_f32(UniformInput &in, uniform_id_t id, float val) {
	this->set_unif(in, id, &val, get_uniform_type_size(GL_FLOAT), GL_FLOAT);
}

void GlShaderProgram::set_f64(UniformInput &in, uniform_id_t id, double val) {
	// TODO requires extension
	this->set_unif(in, id, &val, get_uniform_type_size(GL_DOUBLE), GL_DOUBLE);
}

void GlShaderProgram::set_bool(UniformInput &in, uniform_id_t id, bool val) {
	this->set_unif(in, id, &val, get_uniform_type_size(GL_BOOL), GL_BOOL);
}

void GlShaderProgram::set_v2f32(UniformInput &in, uniform_id_t id, Eigen::Vector2f const &val) {
	this->set_unif(in, id, &val, get_uniform_type_size(GL_FLOAT_VEC2), GL_FLOAT_VEC2);
}

void GlShaderProgram::set_v3f32(UniformInput &in, uniform_id_t id, Eigen::Vector3f const &val) {
	this->set_unif(in, id, &val, get_uniform_type_size(GL_FLOAT_VEC3), GL_FLOAT_VEC3);
}

void GlShaderProgram::set_v4f32(UniformInput &in, uniform_id_t id, Eigen::Vector4f const &val) {
	this->set_unif(in, id, &val, get_uniform_type_size(GL_FLOAT_VEC4), GL_FLOAT_VEC4);
}

void GlShaderProgram::set_v2i32(UniformInput &in, uniform_id_t id, Eigen::Vector2i const &val) {
	this->set_unif(in, id, &val, get_uniform_type_size(GL_INT_VEC2), GL_INT_VEC2);
}

void GlShaderProgram::set_v3i32(UniformInput &in, uniform_id_t id, Eigen::Vector3i const &val) {
	this->set_unif(in, id, &val, get_uniform_type_size(GL_INT_VEC3), GL_INT_VEC3);
}

void GlShaderProgram::set_v4i32(UniformInput &in, uniform_id_t id, Eigen::Vector4i const &val) {
	this->set_unif(in, id, &val, get_uniform_type_size(GL_INT_VEC4), GL_INT_VEC4);
}

void GlShaderProgram::set_v2ui32(UniformInput &in, uniform_id_t id, Eigen::Vector2<uint32_t> const &val) {
	this->set_unif(in, id, &val, get_uniform_type_size(GL_UNSIGNED_INT_VEC2), GL_UNSIGNED_INT_VEC2);
}

void GlShaderProgram::set_v3ui32(UniformInput &in, uniform_id_t id, Eigen::Vector3<uint32_t> const &val) {
	this->set_unif(in, id, &val, get_uniform_type_size(GL_UNSIGNED_INT_VEC3), GL_UNSIGNED_INT_VEC3);
}

void GlShaderProgram::set_v4ui32(UniformInput &in, uniform_id_t id, Eigen::Vector4<uint32_t> const &val) {
	this->set_unif(in, id, &val, get_uniform_type_size(GL_UNSIGNED_INT_VEC4), GL_UNSIGNED_INT_VEC4);
}

void GlShaderProgram::set_m4f32(UniformInput &in, uniform_id_t id, Eigen::Matrix4f const &val) {
	this->set_unif(in, id, val.data(), get_uniform_type_size(GL_FLOAT_MAT4), GL_FLOAT_MAT4);
}

void GlShaderProgram::set_tex(UniformInput &in, uniform_id_t id, std::shared_ptr<Texture2d> const &val) {
	auto tex = std::dynamic_pointer_cast<GlTexture2d>(val);
	GLuint handle = tex->get_handle();
	this->set_unif(in, id, &handle, get_uniform_type_size(GL_SAMPLER_2D), GL_SAMPLER_2D);
}

} // namespace openage::renderer::opengl
