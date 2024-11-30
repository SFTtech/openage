// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#include "util.h"
#include "renderer/uniform_input.h"


namespace openage::renderer::tests {

bool check_uniform_completeness(const std::vector<Renderable> &renderables) {
	bool all_complete = true;

	// Iterate over each renderable object
	for (const auto &renderable : renderables) {
		if (renderable.uniform && !renderable.uniform->is_complete()) {
			all_complete = false;
		}
	}

	return all_complete;
}

} // namespace openage::renderer::tests
