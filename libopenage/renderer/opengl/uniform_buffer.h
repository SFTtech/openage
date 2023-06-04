// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "renderer/opengl/shader_data.h"
#include "renderer/opengl/simple_object.h"
#include "renderer/uniform_buffer.h"

namespace openage::renderer {
class UniformBuffer;
class UniformBufferInput;

namespace opengl {
class GlContext;
class GlUniformInput;
class GlUniformBufferInput;

class GlUniformBuffer final : public UniformBuffer
	, public GlSimpleObject {
public:
	GlUniformBuffer(const std::shared_ptr<GlContext> &context,
	                size_t size,
	                std::unordered_map<std::string, GlInBlockUniform> uniforms,
	                GLuint binding_point = 0,
	                GLenum usage = GL_STATIC_DRAW);

	/**
     * Get the binding point of the buffer.
     *
     * @return Binding point ID.
     */
	GLuint get_binding_point() const;

	/**
     * Set the binding point of the buffer.
     *
     * @param binding_point Binding point ID.
     */
	void set_binding_point(GLuint binding_point);

	void update_uniforms(std::shared_ptr<UniformBufferInput> const &unif_in) override;

	bool has_uniform(const char *) override;

	/**
     * Bind the buffer.
     */
	void bind() const;

protected:
	std::shared_ptr<UniformBufferInput> new_unif_in() override;
	void set_i32(std::shared_ptr<UniformBufferInput> const &, const char *, int32_t) override;
	void set_u32(std::shared_ptr<UniformBufferInput> const &, const char *, uint32_t) override;
	void set_f32(std::shared_ptr<UniformBufferInput> const &, const char *, float) override;
	void set_f64(std::shared_ptr<UniformBufferInput> const &, const char *, double) override;
	void set_bool(std::shared_ptr<UniformBufferInput> const &, const char *, bool) override;
	void set_v2f32(std::shared_ptr<UniformBufferInput> const &, const char *, Eigen::Vector2f const &) override;
	void set_v3f32(std::shared_ptr<UniformBufferInput> const &, const char *, Eigen::Vector3f const &) override;
	void set_v4f32(std::shared_ptr<UniformBufferInput> const &, const char *, Eigen::Vector4f const &) override;
	void set_v2i32(std::shared_ptr<UniformBufferInput> const &, const char *, Eigen::Vector2i const &) override;
	void set_v3i32(std::shared_ptr<UniformBufferInput> const &, const char *, Eigen::Vector3i const &) override;
	void set_v4i32(std::shared_ptr<UniformBufferInput> const &, const char *, Eigen::Vector4i const &) override;
	void set_v2ui32(std::shared_ptr<UniformBufferInput> const &, const char *, Eigen::Vector2<uint32_t> const &) override;
	void set_v3ui32(std::shared_ptr<UniformBufferInput> const &, const char *, Eigen::Vector3<uint32_t> const &) override;
	void set_v4ui32(std::shared_ptr<UniformBufferInput> const &, const char *, Eigen::Vector4<uint32_t> const &) override;
	void set_m4f32(std::shared_ptr<UniformBufferInput> const &, const char *, Eigen::Matrix4f const &) override;
	void set_tex(std::shared_ptr<UniformBufferInput> const &, const char *, std::shared_ptr<Texture2d> const &) override;

private:
	/**
     * Update a uniform value in a uniform buffer input object.
     *
     * Note that the uniform buffer itself is not updated by this. Data is only uploaded
     * to the buffer when \p update_uniforms is eventually called.
     *
     * @param in Uniform buffer input object.
     * @param name Name of the uniform.
     * @param val Pointer to the value to update the uniform with.
     * @param type Type of the uniform.
     */
	void set_unif(std::shared_ptr<UniformBufferInput> const &in,
	              const char *name,
	              void const *val,
	              GLenum type);

	/**
     * Uniform definitions inside the buffer.
     */
	std::unordered_map<std::string, GlInBlockUniform> uniforms;

	/**
     * Size of the buffer (in bytes).
     */
	size_t data_size;

	/**
     * Binding point of the buffer.
     */
	GLuint binding_point;
};

} // namespace opengl
} // namespace openage::renderer
