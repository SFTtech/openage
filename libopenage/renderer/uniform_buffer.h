// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <memory>

#include <eigen3/Eigen/Dense>

#include "renderer/uniform_input.h"


namespace openage::renderer {
class Texture2d;
class UniformBufferInput;

class UniformBuffer : public std::enable_shared_from_this<UniformBuffer> {
	friend UniformBufferInput;

public:
	virtual ~UniformBuffer() = default;

	/**
	 * Update the uniforms in the buffer.
	 *
	 * @param unif_in Uniform input to update the buffer with.
	 */
	virtual void update_uniforms(std::shared_ptr<UniformBufferInput> const &unif_in) = 0;

	/**
	 * Check whether the buffer contains a uniform variable with the given ID.
	 *
	 * @param unif ID of the uniform.
	 *
	 * @return true if the buffer contains the uniform, false otherwise.
	 */
	virtual bool has_uniform(const char *unif) = 0;

	template <typename... Ts>
	std::shared_ptr<UniformBufferInput> new_uniform_input(Ts &&...vals) {
		auto input = this->new_unif_in();
		input->update(vals...);
		return input;
	}

	std::shared_ptr<UniformBufferInput> create_empty_input() {
		return this->new_uniform_input();
	}

protected:
	virtual std::shared_ptr<UniformBufferInput> new_unif_in() = 0;

	virtual void set_i32(UniformBufferInput &in, const char *, int32_t) = 0;
	virtual void set_u32(UniformBufferInput &in, const char *, uint32_t) = 0;
	virtual void set_f32(UniformBufferInput &in, const char *, float) = 0;
	virtual void set_f64(UniformBufferInput &in, const char *, double) = 0;
	virtual void set_bool(UniformBufferInput &in, const char *, bool) = 0;
	virtual void set_v2f32(UniformBufferInput &in, const char *, Eigen::Vector2f const &) = 0;
	virtual void set_v3f32(UniformBufferInput &in, const char *, Eigen::Vector3f const &) = 0;
	virtual void set_v4f32(UniformBufferInput &in, const char *, Eigen::Vector4f const &) = 0;
	virtual void set_v2i32(UniformBufferInput &in, const char *, Eigen::Vector2i const &) = 0;
	virtual void set_v3i32(UniformBufferInput &in, const char *, Eigen::Vector3i const &) = 0;
	virtual void set_v4i32(UniformBufferInput &in, const char *, Eigen::Vector4i const &) = 0;
	virtual void set_v2ui32(UniformBufferInput &in, const char *, Eigen::Vector2<uint32_t> const &) = 0;
	virtual void set_v3ui32(UniformBufferInput &in, const char *, Eigen::Vector3<uint32_t> const &) = 0;
	virtual void set_v4ui32(UniformBufferInput &in, const char *, Eigen::Vector4<uint32_t> const &) = 0;
	virtual void set_m4f32(UniformBufferInput &in, const char *, Eigen::Matrix4f const &) = 0;
	virtual void set_tex(UniformBufferInput &in, const char *, std::shared_ptr<Texture2d> const &) = 0;
};

} // namespace openage::renderer
