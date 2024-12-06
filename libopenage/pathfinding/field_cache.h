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

class FieldCache
{
public:
    FieldCache() = default;
    ~FieldCache() = default;

    void add(std::pair<unsigned long long, unsigned long long> cache_key,
             std::shared_ptr<IntegrationField> &integration_field,
             std::shared_ptr<FlowField> &flow_field);

	void evict(std::pair<unsigned long long, unsigned long long> cache_key);

    bool is_cached(std::pair<unsigned long long, unsigned long long> cache_key);

    std::shared_ptr<IntegrationField> get_integration_field(std::pair<unsigned long long, unsigned long long> cache_key);
    
    std::shared_ptr<FlowField> get_flow_field(std::pair<unsigned long long, unsigned long long> cache_key);
    
    using get_return_t = std::pair<std::shared_ptr<IntegrationField>, std::shared_ptr<FlowField>>;

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
	std::unordered_map<std::pair<portal_id_t, sector_id_t>,
	                   get_return_t,
	                   pair_hash>
		cache;
};

} // namespace path
} // namespace openage
