// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

namespace openage::renderer::resources {

class BlendPatternInfo;

/**
 * Contains information about a blending table.
 */
class BlendTableInfo {
public:
	/**
	 * Create a blending table Info.
	 *
	 * @param table Lookup table for blending pattern IDs.
	 * @param patterns Blending pattern information.
	 */
	BlendTableInfo(const std::vector<size_t> &table,
	               const std::vector<std::shared_ptr<BlendPatternInfo>> &patterns);

	BlendTableInfo() = default;
	~BlendTableInfo() = default;

	/**
	 * Get the blending lookup table.
	 *
	 * @return Lookup table for blending patterns.
	 */
	const std::vector<size_t> &get_table() const;

	/**
	 * Get the lending pattern information of the pattern with the specified index.
	 *
	 * @param idx Index of the pattern.
	 *
	 * @return Blending pattern information object.
	 */
	const std::shared_ptr<BlendPatternInfo> &get_pattern(size_t idx) const;

private:
	/**
	 * Lookup table for blending pattern IDs.
	 */
	std::vector<size_t> table;

	/**
	 * Blending pattern information.
	 */
	std::vector<std::shared_ptr<BlendPatternInfo>> patterns;
};

} // namespace openage::renderer::resources
