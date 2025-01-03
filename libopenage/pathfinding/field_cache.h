// Copyright 2024-2024 the openage authors. See copying.md for legal info.

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
	 * Adds a new field cache entry to the cache with a given portal and sector cache key.
	 */
	void add(cache_key_t cache_key,
	         field_cache_t cache_entry);

	/**
	 * Evicts a given field cache entry from the cache at the given cache key.
	 */
	void evict(cache_key_t cache_key);

	/**
	 * Checks if there is a cached entry at a specific cache key.
	 */
	bool is_cached(cache_key_t cache_key);

	/**
	 * Gets the integration field from a given cache entry.
	 */
	std::shared_ptr<IntegrationField> get_integration_field(cache_key_t cache_key);

	/**
	 * Gets the flow field from a given cache entry.
	 */
	std::shared_ptr<FlowField> get_flow_field(cache_key_t cache_key);

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
