// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <unordered_map>

#include "pathfinding/types.h"
#include "util/hash.h"


namespace openage {
namespace coord {
struct tile_delta;
} // namespace coord

namespace path {
class IntegrationField;
class FlowField;

/**
 * Cache to store already calculated flow and integration fields for the pathfinding algorithm.
 */
class FieldCache {
public:
	FieldCache() = default;
	~FieldCache() = default;

	/**
	 * Adds a new field entry to the cache.
	 *
	 * @param cache_key Cache key for the field entry.
	 * @param cache_entry Field entry (integration field, flow field).
	 */
	void add(const cache_key_t cache_key,
	         const field_cache_t cache_entry);

	/**
	 * Evict field entry from the cache.
	 *
	 * @param cache_key Cache key for the field entry to evict.
	 *
	 * @return true if the cache key was found and evicted, false otherwise.
	 */
	bool evict(const cache_key_t cache_key);

	/**
	 * Check if there is a cached entry for a specific cache key.
	 *
	 * @param cache_key Cache key to check.
	 *
	 * @return true if a field entry is found for the cache key, false otherwise.
	 */
	bool is_cached(const cache_key_t cache_key) const;

	/**
	 * Get a cached integration field.
	 *
	 * @param cache_key Cache key for the field entry.
	 *
	 * @return Integration field.
	 */
	std::shared_ptr<IntegrationField> get_integration_field(const cache_key_t cache_key) const;

	/**
	 * Get a cached flow field.
	 *
	 * @param cache_key Cache key for the field entry.
	 *
	 * @return Flow field.
	 */
	std::shared_ptr<FlowField> get_flow_field(const cache_key_t cache_key) const;

	/**
	 * Get a cached field entry.
	 *
	 * Contains both integration field and flow field.
	 *
	 * @param cache_key Cache key for the field entry.
	 *
	 * @return Field entry (integration field, flow field).
	 */
	field_cache_t get(const cache_key_t cache_key) const;

private:
	/**
	 * Hash function for the field cache.
	 */
	struct pair_hash {
		template <class T1, class T2>
		std::size_t operator()(const std::pair<T1, T2> &pair) const {
			return util::hash_combine(std::hash<T1>{}(pair.first), std::hash<T2>{}(pair.second));
		}
	};

	/**
	 * Cache for already computed fields.
	 *
	 * Key is the portal ID and the sector ID from which the field was entered. Fields that are cached are
	 * cleared of dynamic flags, i.e. wavefront or LOS flags. These have to be recalculated
	 * when the field is reused.
	 */
	std::unordered_map<cache_key_t,
	                   field_cache_t,
	                   pair_hash>
		cache;
};

} // namespace path
} // namespace openage
