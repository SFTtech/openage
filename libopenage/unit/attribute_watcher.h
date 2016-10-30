// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>

namespace openage {

class AttributeWatcher {
public:
	template<typename T>
	void apply(id_t id, T value, const char *name) {
		std::cout << id << " " << name << " " << value << "\n";
	}
};

} // namespace openage
