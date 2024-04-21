// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "buffer_info.h"

namespace openage::renderer::resources {

UniformBufferInfo::UniformBufferInfo(ubo_layout_t layout,
                                     const std::vector<UBOInput> &inputs) :
	layout{layout},
	inputs{inputs} {
}

UniformBufferInfo::UniformBufferInfo(ubo_layout_t layout,
                                     std::vector<UBOInput> &&inputs) :
	layout{layout},
	inputs{inputs} {
}

ubo_layout_t UniformBufferInfo::get_layout() const {
	return this->layout;
}

const std::vector<UBOInput> &UniformBufferInfo::get_inputs() const {
	return this->inputs;
}

size_t UniformBufferInfo::get_size() const {
	size_t size = 0;
	for (const auto &input : this->inputs) {
		// size of the input type
		size_t input_size = this->get_size(input, this->layout);

		// inputs must additionally be aligned to a multiple of their size
		size_t align_size = size % input_size;

		size += input_size + align_size;
	}
	return size;
}

size_t UniformBufferInfo::get_size(const UBOInput &input, ubo_layout_t /* layout */) {
	if (input.count == 1) {
		return STD140_INPUT_SIZE.get(input.type);
	}

	// length of single element is always padded to vec4
	return STD140_INPUT_SIZE.get(ubo_input_t::V4F32) * input.count;
}

size_t UniformBufferInfo::get_stride_size(ubo_input_t /* input */, ubo_layout_t /* layout */) {
	return STD140_INPUT_SIZE.get(ubo_input_t::V4F32);
}


} // namespace openage::renderer::resources
