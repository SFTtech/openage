// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "blendtable_info.h"

namespace openage::renderer::resources {

BlendTableInfo::BlendTableInfo(const std::vector<size_t> &table,
                               const std::vector<std::shared_ptr<BlendPatternInfo>> &patterns) :
	table{table},
	patterns{patterns} {}

const std::vector<size_t> &BlendTableInfo::get_table() const {
	return this->table;
}

const std::shared_ptr<BlendPatternInfo> &BlendTableInfo::get_pattern(size_t idx) const {
	return this->patterns.at(idx);
}

} // namespace openage::renderer::resources
