// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "integrator.h"

#include "log/log.h"

#include "pathfinding/cost_field.h"
#include "pathfinding/flow_field.h"
#include "pathfinding/integration_field.h"
#include "pathfinding/portal.h"


namespace openage::path {

std::shared_ptr<IntegrationField> Integrator::integrate(const std::shared_ptr<CostField> &cost_field,
                                                        const coord::tile_delta &target,
                                                        bool with_los) {
	auto integration_field = std::make_shared<IntegrationField>(cost_field->get_size());

	log::log(DBG << "Integrating cost field for target coord " << target);
	if (with_los) {
		log::log(SPAM << "Performing LOS pass");
		auto wavefront_blocked = integration_field->integrate_los(cost_field, target);
		integration_field->integrate_cost(cost_field, std::move(wavefront_blocked));
	}
	else {
		log::log(SPAM << "Skipping LOS pass");
		integration_field->integrate_cost(cost_field, target);
	}

	return integration_field;
}

std::shared_ptr<IntegrationField> Integrator::integrate(const std::shared_ptr<CostField> &cost_field,
                                                        const std::shared_ptr<IntegrationField> &other,
                                                        sector_id_t other_sector_id,
                                                        const std::shared_ptr<Portal> &portal,
                                                        const coord::tile_delta &target,
                                                        bool with_los) {
	auto cache_key = std::make_pair(portal->get_id(), other_sector_id);
	auto cached = this->field_cache.find(cache_key);
	if (cached != this->field_cache.end()) {
		log::log(DBG << "Using cached integration field for portal " << portal->get_id()
		             << " from sector " << other_sector_id);
		if (with_los) {
			log::log(SPAM << "Performing LOS pass on cached field");

			// Make a copy of the cached field to avoid modifying the cached field
			auto integration_field = std::make_shared<IntegrationField>(*cached->second.first);

			// Only integrate LOS; leave the rest of the field as is
			integration_field->integrate_los(cost_field, other, other_sector_id, portal, target);

			return integration_field;
		}
		return cached->second.first;
	}

	log::log(DBG << "Integrating cost field for portal " << portal->get_id()
	             << " from sector " << other_sector_id);

	// Create a new integration field
	auto integration_field = std::make_shared<IntegrationField>(cost_field->get_size());

	// LOS pass
	std::vector<size_t> wavefront_blocked;
	if (with_los) {
		log::log(SPAM << "Performing LOS pass");
		wavefront_blocked = integration_field->integrate_los(cost_field, other, other_sector_id, portal, target);
	}

	// Cost integration
	if (wavefront_blocked.empty()) {
		// No LOS pass or no blocked cells
		// use the portal as the target
		integration_field->integrate_cost(cost_field, other_sector_id, portal);
	}
	else {
		// LOS pass was performed and some cells were blocked
		// use the blocked cells as the start wave
		integration_field->integrate_cost(cost_field, std::move(wavefront_blocked));
	}

	return integration_field;
}

std::shared_ptr<FlowField> Integrator::build(const std::shared_ptr<IntegrationField> &integration_field) {
	auto flow_field = std::make_shared<FlowField>(integration_field->get_size());

	log::log(DBG << "Building flow field from integration field");
	flow_field->build(integration_field);

	return flow_field;
}

std::shared_ptr<FlowField> Integrator::build(const std::shared_ptr<IntegrationField> &integration_field,
                                             const std::shared_ptr<IntegrationField> &other,
                                             sector_id_t other_sector_id,
                                             const std::shared_ptr<Portal> &portal,
                                             bool with_los) {
	auto cache_key = std::make_pair(portal->get_id(), other_sector_id);
	auto cached = this->field_cache.find(cache_key);
	if (cached != this->field_cache.end()) {
		log::log(DBG << "Using cached flow field for portal " << portal->get_id()
		             << " from sector " << other_sector_id);
		if (with_los) {
			log::log(SPAM << "Transferring LOS flags to cached flow field");

			// Make a copy of the cached flow field
			auto flow_field = std::make_shared<FlowField>(*cached->second.second);

			// Transfer the LOS flags to the flow field
			flow_field->transfer_dynamic_flags(integration_field);

			return flow_field;
		}

		return cached->second.second;
	}

	log::log(DBG << "Building flow field for portal " << portal->get_id()
	             << " from sector " << other_sector_id);

	auto flow_field = std::make_shared<FlowField>(integration_field->get_size());
	flow_field->build(integration_field, other, other_sector_id, portal);

	return flow_field;
}

Integrator::get_return_t Integrator::get(const std::shared_ptr<CostField> &cost_field,
                                         const coord::tile_delta &target) {
	auto integration_field = this->integrate(cost_field, target);
	auto flow_field = this->build(integration_field);

	return std::make_pair(integration_field, flow_field);
}

Integrator::get_return_t Integrator::get(const std::shared_ptr<CostField> &cost_field,
                                         const std::shared_ptr<IntegrationField> &other,
                                         sector_id_t other_sector_id,
                                         const std::shared_ptr<Portal> &portal,
                                         const coord::tile_delta &target,
                                         bool with_los) {
	auto cache_key = std::make_pair(portal->get_id(), other_sector_id);
	auto cached = this->field_cache.find(cache_key);
	if (cached != this->field_cache.end()) {
		log::log(DBG << "Using cached integration and flow fields for portal " << portal->get_id()
		             << " from sector " << other_sector_id);
		if (with_los) {
			log::log(SPAM << "Performing LOS pass on cached field");

			// Make a copy of the cached integration field
			auto integration_field = std::make_shared<IntegrationField>(*cached->second.first);

			// Only integrate LOS; leave the rest of the field as is
			integration_field->integrate_los(cost_field, other, other_sector_id, portal, target);

			log::log(SPAM << "Transferring LOS flags to cached flow field");

			// Make a copy of the cached flow field
			auto flow_field = std::make_shared<FlowField>(*cached->second.second);

			// Transfer the LOS flags to the flow field
			flow_field->transfer_dynamic_flags(integration_field);

			return std::make_pair(integration_field, flow_field);
		}

		return cached->second;
	}

	auto integration_field = this->integrate(cost_field, other, other_sector_id, portal, target, with_los);
	auto flow_field = this->build(integration_field, other, other_sector_id, portal);

	log::log(DBG << "Caching integration and flow fields for portal ID: " << portal->get_id()
	             << ", sector ID: " << other_sector_id);

	// Copy the fields to the cache.
	std::shared_ptr<IntegrationField> cached_integration_field = std::make_shared<IntegrationField>(*integration_field);
	cached_integration_field->reset_dynamic_flags();

	std::shared_ptr<FlowField> cached_flow_field = std::make_shared<FlowField>(*flow_field);
	cached_flow_field->reset_dynamic_flags();

	this->field_cache[cache_key] = std::make_pair(cached_integration_field, cached_flow_field);

	return std::make_pair(integration_field, flow_field);
}

} // namespace openage::path
