// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#include "field_cache.h"

namespace openage::path {

void FieldCache::add(const cache_key_t cache_key,
                     const field_cache_t cache_entry) {
	this->cache[cache_key] = cache_entry;
}

bool FieldCache::evict(const cache_key_t cache_key) {
	return this->cache.erase(cache_key) != 0;
}

bool FieldCache::is_cached(const cache_key_t cache_key) const {
	return this->cache.contains(cache_key);
}

std::shared_ptr<IntegrationField> FieldCache::get_integration_field(const cache_key_t cache_key) const {
	return this->cache.at(cache_key).first;
}

std::shared_ptr<FlowField> FieldCache::get_flow_field(const cache_key_t cache_key) const {
	return this->cache.at(cache_key).second;
}

field_cache_t FieldCache::get(const cache_key_t cache_key) const {
	return this->cache.at(cache_key);
}

} // namespace openage::path
