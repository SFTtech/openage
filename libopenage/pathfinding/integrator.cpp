// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "integrator.h"

#include "pathfinding/cost_field.h"
#include "pathfinding/flow_field.h"
#include "pathfinding/integration_field.h"
#include "pathfinding/portal.h"


namespace openage::path {

std::shared_ptr<IntegrationField> Integrator::integrate(const std::shared_ptr<CostField> &cost_field,
                                                        const coord::tile_delta &target) {
	auto integration_field = std::make_shared<IntegrationField>(cost_field->get_size());

	auto wavefront_blocked = integration_field->integrate_los(cost_field, target);
	integration_field->integrate_cost(cost_field, std::move(wavefront_blocked));

	return integration_field;
}

std::shared_ptr<IntegrationField> Integrator::integrate(const std::shared_ptr<CostField> &cost_field,
                                                        sector_id_t other_sector_id,
                                                        const std::shared_ptr<Portal> &portal) {
	auto cache_key = std::make_pair(portal->get_id(), other_sector_id);
	auto cached = this->field_cache.find(cache_key);
	if (cached != this->field_cache.end()) {
		return cached->second.first;
	}

	auto integration_field = std::make_shared<IntegrationField>(cost_field->get_size());
	integration_field->integrate_cost(cost_field, other_sector_id, portal);

	return integration_field;
}

std::shared_ptr<FlowField> Integrator::build(const std::shared_ptr<IntegrationField> &integration_field) {
	auto flow_field = std::make_shared<FlowField>(integration_field->get_size());
	flow_field->build(integration_field);

	return flow_field;
}

std::shared_ptr<FlowField> Integrator::build(const std::shared_ptr<IntegrationField> &integration_field,
                                             const std::shared_ptr<IntegrationField> &other,
                                             sector_id_t other_sector_id,
                                             const std::shared_ptr<Portal> &portal) {
	auto cache_key = std::make_pair(portal->get_id(), other_sector_id);
	auto cached = this->field_cache.find(cache_key);
	if (cached != this->field_cache.end()) {
		return cached->second.second;
	}

	auto flow_field = std::make_shared<FlowField>(integration_field->get_size());
	flow_field->build(integration_field, other, other_sector_id, portal);

	return flow_field;
}

Integrator::build_return_t Integrator::build(const std::shared_ptr<CostField> &cost_field,
                                             const coord::tile_delta &target) {
	auto integration_field = this->integrate(cost_field, target);
	auto flow_field = this->build(integration_field);

	return std::make_pair(integration_field, flow_field);
}

Integrator::build_return_t Integrator::build(const std::shared_ptr<CostField> &cost_field,
                                             const std::shared_ptr<IntegrationField> &other_integration_field,
                                             sector_id_t other_sector_id,
                                             const std::shared_ptr<Portal> &portal) {
	auto cache_key = std::make_pair(portal->get_id(), other_sector_id);
	auto cached = this->field_cache.find(cache_key);
	if (cached != this->field_cache.end()) {
		return cached->second;
	}

	auto integration_field = this->integrate(cost_field, other_sector_id, portal);
	auto flow_field = this->build(integration_field, other_integration_field, other_sector_id, portal);

	// Copy the fields to the cache.
	std::shared_ptr<IntegrationField> cached_integration_field = std::make_shared<IntegrationField>(*integration_field);
	cached_integration_field->reset_dynamic_flags();
	std::shared_ptr<FlowField> cached_flow_field = std::make_shared<FlowField>(*flow_field);
	cached_flow_field->reset_dynamic_flags();
	this->field_cache[cache_key] = std::make_pair(cached_integration_field, cached_flow_field);

	return std::make_pair(integration_field, flow_field);
}

} // namespace openage::path
