// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "datastructure/constexpr_map.h"


namespace openage::renderer::resources {

/**
 * Uniform buffer layout types.
 *
 * std140 is the only layout for which we can predict the size,
 * so it is the only one we support.
 */
enum class ubo_layout_t {
	STD140,
};

/**
 * Uniform types.
 */
enum class ubo_input_t {
	I32,
	U32,
	F32,
	F64,
	BOOL,
	V2F32,
	V3F32,
	V4F32,
	V2I32,
	V3I32,
	V4I32,
	V2U32,
	V3U32,
	V4U32,
	M4F32,
};

/**
 * Uniform block input definition.
 */
struct UBOInput {
	// Uniform name.
	std::string name;
	// Uniform type.
	ubo_input_t type;
	// Length (values >1 are arrays)
	uint32_t count = 1;
};


/**
 * Size of uniform input types in std140 layout _including_ padding (in bytes).
 */
static constexpr auto STD140_INPUT_SIZE = datastructure::create_const_map<ubo_input_t, size_t>(
	std::pair(ubo_input_t::I32, 4),
	std::pair(ubo_input_t::U32, 4),
	std::pair(ubo_input_t::F32, 4),
	std::pair(ubo_input_t::F64, 8),
	std::pair(ubo_input_t::BOOL, 4),
	std::pair(ubo_input_t::V2F32, 8),
	std::pair(ubo_input_t::V3F32, 16),
	std::pair(ubo_input_t::V4F32, 16),
	std::pair(ubo_input_t::V2I32, 8),
	std::pair(ubo_input_t::V3I32, 16),
	std::pair(ubo_input_t::V4I32, 16),
	std::pair(ubo_input_t::V2U32, 8),
	std::pair(ubo_input_t::V3U32, 16),
	std::pair(ubo_input_t::V4U32, 16),
	std::pair(ubo_input_t::M4F32, 64));


class UniformBufferInfo {
public:
	/**
	 * Create a new uniform buffer definition.
	 *
	 * @param layout Layout of the uniform block targeted by the buffer.
	 * @param inputs Uniform definitions inside the uniform block.
	 */
	UniformBufferInfo(ubo_layout_t layout, const std::vector<UBOInput> &inputs);

	/**
	 * Create a new uniform buffer definition.
	 *
	 * @param layout Layout of the uniform block targeted by the buffer.
	 * @param inputs Uniform definitions inside the uniform block.
	 */
	UniformBufferInfo(ubo_layout_t layout, std::vector<UBOInput> &&inputs);

	/**
	 * Get the layout of the uniform block targeted by the buffer.
	 *
	 * @return Uniform block layout.
	 */
	ubo_layout_t get_layout() const;

	/**
	 * Get the uniform definitions inside the uniform block.
	 *
	 * @return Uniform definitions.
	 */
	const std::vector<UBOInput> &get_inputs() const;

	/**
	 * Get the total size of the uniform block (in bytes).
	 *
	 * @return Size of the uniform block (in bytes).
	 */
	size_t get_size() const;

	/**
	 * Get the size of a single uniform input (in bytes).
	 *
	 * @param input Uniform input type.
	 * @param layout Layout of the uniform block targeted by the buffer.
	 *
	 * @return Size of the uniform input (in bytes).
	 */
	static size_t get_size(const UBOInput &input, ubo_layout_t layout = ubo_layout_t::STD140);

	/**
	 * Get the stride size of a uniform input in an array (in bytes).
	 *
	 * @param input Uniform input type.
	 * @param layout Layout of the uniform block targeted by the buffer.
	 *
	 * @return Size of the uniform input (in bytes).
	 */
	static size_t get_stride_size(ubo_input_t input, ubo_layout_t layout = ubo_layout_t::STD140);

private:
	/**
	 * Uniform block layout.
	 */
	ubo_layout_t layout;

	/**
	 * Uniform definitions.
	 */
	std::vector<UBOInput> inputs;
};

} // namespace openage::renderer::resources
