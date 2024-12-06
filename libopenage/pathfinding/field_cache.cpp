// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "field_cache.h"

namespace openage::path {

void FieldCache::add(std::pair<unsigned long long, unsigned long long> cache_key,
            std::shared_ptr<IntegrationField> &integration_field,
            std::shared_ptr<FlowField> &flow_field) {
    this->cache[cache_key] = std::make_pair(integration_field, flow_field);
}

void FieldCache::evict(std::pair<unsigned long long, unsigned long long> cache_key) {
    this->cache.erase(cache_key);
}

bool FieldCache::is_cached(std::pair<unsigned long long, unsigned long long> cache_key) {
    return this->cache.contains(cache_key);
}

std::shared_ptr<IntegrationField> FieldCache::get_integration_field(std::pair<unsigned long long, unsigned long long> cache_key) {
    auto cached = this->cache.find(cache_key);
    return cached->second.first;
}

std::shared_ptr<FlowField> FieldCache::get_flow_field(std::pair<unsigned long long, unsigned long long> cache_key) {
    auto cached = this->cache.find(cache_key);
    return cached->second.second;
}

} // namespace openage::path
