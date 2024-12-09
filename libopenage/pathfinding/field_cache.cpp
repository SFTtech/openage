// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "field_cache.h"

namespace openage::path {

void FieldCache::add(cache_key_t cache_key,
                     field_cache_t cache_entry) {
	this->cache[cache_key] = cache_entry;
}

void FieldCache::evict(cache_key_t cache_key) {
	this->cache.erase(cache_key);
}

bool FieldCache::is_cached(cache_key_t cache_key) {
	return this->cache.contains(cache_key);
}

std::shared_ptr<IntegrationField> FieldCache::get_integration_field(cache_key_t cache_key) {
	auto cached = this->cache.find(cache_key);
	return cached->second.first;
}

std::shared_ptr<FlowField> FieldCache::get_flow_field(cache_key_t cache_key) {
	auto cached = this->cache.find(cache_key);
	return cached->second.second;
}

} // namespace openage::path
