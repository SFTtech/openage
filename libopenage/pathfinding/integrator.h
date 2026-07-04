// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "pathfinding/cost_field.h"
#include "pathfinding/field_cache.h"
#include "pathfinding/flow_field.h"
#include "pathfinding/integration_field.h"
#include "pathfinding/portal.h"
#include "pathfinding/types.h"

#include "time/time.h"
#include "util/hash.h"

#include "log/log.h"


namespace openage {
namespace coord {
struct tile_delta;
} // namespace coord

namespace path {

template <size_t SECTOR_SIDE_LENGTH>
class CostField;

template <size_t SECTOR_SIDE_LENGTH>
class FlowField;

template <size_t SECTOR_SIDE_LENGTH>
class IntegrationField;

class Portal;

/**
 * Integrator for the flow field pathfinding algorithm.
 */
template <size_t SECTOR_SIDE_LENGTH>
class Integrator {
public:
	/**
	 * Create a new integrator.
	 */
	Integrator();

	~Integrator() = default;

	/**
	 * Integrate the cost field for a target.
	 *
	 * This should be used for the field containing the target cell.
	 * The target coordinates must lie within the boundaries of the cost field.
	 *
	 * @param cost_field Cost field.
	 * @param target Coordinates of the target cell.
	 * @param with_los If true an LOS pass is performed before cost integration.
	 *
	 * @return Integration field.
	 */
	std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> integrate(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
	                                                                const coord::tile_delta &target,
	                                                                bool with_los = true);

	/**
	 * Integrate the cost field from a portal.
	 *
	 * This should be used for the fields on the portal path that are not the target field.
	 * The target coordinates must be relative to the origin of the sector the cost field belongs to.
	 *
	 * @param cost_field Cost field.
	 * @param other Integration field of the other side of the portal.
	 * @param other_sector_id Sector ID of the other side of the portal.
	 * @param portal Portal.
	 * @param target Coordinates of the target cell, relative to the integration field origin.
	 * @param time Time of the path request.
	 * @param with_los If true an LOS pass is performed before cost integration.
	 *
	 * @return Integration field.
	 */
	std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> integrate(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
	                                                                const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &other,
	                                                                sector_id_t other_sector_id,
	                                                                const std::shared_ptr<Portal> &portal,
	                                                                const coord::tile_delta &target,
	                                                                const time::time_t &time,
	                                                                bool with_los = true);

	/**
	 * Build the flow field from an integration field.
	 *
	 * @param integration_field Integration field.
	 *
	 * @return Flow field.
	 */
	std::shared_ptr<FlowField<SECTOR_SIDE_LENGTH>> build(const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &integration_field);

	/**
	 * Build the flow field from a portal.
	 *
	 * @param integration_field Integration field.
	 * @param other Integration field of the other side of the portal.
	 * @param other_sector_id Sector ID of the other side of the portal.
	 * @param portal Portal.
	 * @param with_los If true LOS flags are calculated if the flow field is in cache.
	 *
	 * @return Flow field.
	 */
	std::shared_ptr<FlowField<SECTOR_SIDE_LENGTH>> build(const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &integration_field,
	                                                     const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &other,
	                                                     sector_id_t other_sector_id,
	                                                     const std::shared_ptr<Portal> &portal,
	                                                     bool with_los = true);

	using get_return_t = std::pair<std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>>, std::shared_ptr<FlowField<SECTOR_SIDE_LENGTH>>>;

	/**
	 * Get the integration field and flow field for a target.
	 *
	 * @param cost_field Cost field.
	 * @param target Coordinates of the target cell.
	 *
	 * @return Integration field and flow field.
	 */
	get_return_t get(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
	                 const coord::tile_delta &target);

	/**
	 * Get the integration field and flow field from a portal.
	 *
	 * @param cost_field Cost field.
	 * @param other Integration field of the other side of the portal.
	 * @param other_sector_id Sector ID of the other side of the portal.
	 * @param portal Portal.
	 * @param target Coordinates of the target cell, relative to the integration field origin.
	 * @param time Time of the path request.
	 * @param with_los If true an LOS pass is performed before cost integration.
	 *
	 * @return Integration field and flow field.
	 */
	get_return_t get(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
	                 const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &other,
	                 sector_id_t other_sector_id,
	                 const std::shared_ptr<Portal> &portal,
	                 const coord::tile_delta &target,
	                 const time::time_t &time,
	                 bool with_los = true);

private:
	/**
	 * Cache for already computed fields.
	 */
	std::unique_ptr<FieldCache<SECTOR_SIDE_LENGTH>> field_cache;
};

template <size_t SECTOR_SIDE_LENGTH>
Integrator<SECTOR_SIDE_LENGTH>::Integrator() :
	field_cache{std::make_unique<FieldCache<SECTOR_SIDE_LENGTH>>()} {
}

template <size_t SECTOR_SIDE_LENGTH>
std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> Integrator<SECTOR_SIDE_LENGTH>::integrate(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
                                                                                                const coord::tile_delta &target,
                                                                                                bool with_los) {
	auto integration_field = std::make_shared<IntegrationField<SECTOR_SIDE_LENGTH>>();

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

template <size_t SECTOR_SIDE_LENGTH>
std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> Integrator<SECTOR_SIDE_LENGTH>::integrate(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
                                                                                                const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &other,
                                                                                                sector_id_t other_sector_id,
                                                                                                const std::shared_ptr<Portal> &portal,
                                                                                                const coord::tile_delta &target,
                                                                                                const time::time_t &time,
                                                                                                bool with_los) {
	auto cache_key = std::make_pair(portal->get_id(), other_sector_id);
	if (cost_field->is_dirty(time)) {
		log::log(DBG << "Evicting cached integration and flow fields for portal " << portal->get_id()
		             << " from sector " << other_sector_id);
		this->field_cache->evict(cache_key);
	}
	else if (this->field_cache->is_cached(cache_key)) {
		log::log(DBG << "Using cached integration field for portal " << portal->get_id()
		             << " from sector " << other_sector_id);

		// retrieve cached integration field
		auto cached_integration_field = this->field_cache->get_integration_field(cache_key);

		if (with_los) {
			log::log(SPAM << "Performing LOS pass on cached field");

			// Make a copy of the cached field to avoid modifying the cached field
			auto integration_field = std::make_shared<IntegrationField<SECTOR_SIDE_LENGTH>>(*cached_integration_field);

			// Only integrate LOS; leave the rest of the field as is
			integration_field->integrate_los(cost_field, other, other_sector_id, portal, target);

			return integration_field;
		}
		return cached_integration_field;
	}

	log::log(DBG << "Integrating cost field for portal " << portal->get_id()
	             << " from sector " << other_sector_id);

	// Create a new integration field
	auto integration_field = std::make_shared<IntegrationField<SECTOR_SIDE_LENGTH>>();

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

template <size_t SECTOR_SIDE_LENGTH>
std::shared_ptr<FlowField<SECTOR_SIDE_LENGTH>> Integrator<SECTOR_SIDE_LENGTH>::build(const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &integration_field) {
	auto flow_field = std::make_shared<FlowField<SECTOR_SIDE_LENGTH>>();

	log::log(DBG << "Building flow field from integration field");
	flow_field->build(integration_field);

	return flow_field;
}

template <size_t SECTOR_SIDE_LENGTH>
std::shared_ptr<FlowField<SECTOR_SIDE_LENGTH>> Integrator<SECTOR_SIDE_LENGTH>::build(const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &integration_field,
                                                                                     const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &other,
                                                                                     sector_id_t other_sector_id,
                                                                                     const std::shared_ptr<Portal> &portal,
                                                                                     bool with_los) {
	auto cache_key = std::make_pair(portal->get_id(), other_sector_id);
	if (this->field_cache->is_cached(cache_key)) {
		log::log(DBG << "Using cached flow field for portal " << portal->get_id()
		             << " from sector " << other_sector_id);

		// retrieve cached flow field
		auto cached_flow_field = this->field_cache->get_flow_field(cache_key);

		if (with_los) {
			log::log(SPAM << "Transferring LOS flags to cached flow field");

			// Make a copy of the cached flow field
			auto flow_field = std::make_shared<FlowField<SECTOR_SIDE_LENGTH>>(*cached_flow_field);

			// Transfer the LOS flags to the flow field
			flow_field->transfer_dynamic_flags(integration_field);

			return flow_field;
		}

		return cached_flow_field;
	}

	log::log(DBG << "Building flow field for portal " << portal->get_id()
	             << " from sector " << other_sector_id);

	auto flow_field = std::make_shared<FlowField<SECTOR_SIDE_LENGTH>>();
	flow_field->build(integration_field, other, other_sector_id, portal);

	return flow_field;
}

template <size_t SECTOR_SIDE_LENGTH>
Integrator<SECTOR_SIDE_LENGTH>::get_return_t Integrator<SECTOR_SIDE_LENGTH>::get(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
                                                                                 const coord::tile_delta &target) {
	auto integration_field = this->integrate(cost_field, target);
	auto flow_field = this->build(integration_field);

	return std::make_pair(integration_field, flow_field);
}

template <size_t SECTOR_SIDE_LENGTH>
Integrator<SECTOR_SIDE_LENGTH>::get_return_t Integrator<SECTOR_SIDE_LENGTH>::get(const std::shared_ptr<CostField<SECTOR_SIDE_LENGTH>> &cost_field,
                                                                                 const std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> &other,
                                                                                 sector_id_t other_sector_id,
                                                                                 const std::shared_ptr<Portal> &portal,
                                                                                 const coord::tile_delta &target,
                                                                                 const time::time_t &time,
                                                                                 bool with_los) {
	auto cache_key = std::make_pair(portal->get_id(), other_sector_id);
	if (cost_field->is_dirty(time)) {
		log::log(DBG << "Evicting cached integration and flow fields for portal " << portal->get_id()
		             << " from sector " << other_sector_id);
		this->field_cache->evict(cache_key);
	}
	else if (this->field_cache->is_cached(cache_key)) {
		log::log(DBG << "Using cached integration and flow fields for portal " << portal->get_id()
		             << " from sector " << other_sector_id);

		// retrieve cached fields
		auto cached_fields = this->field_cache->get(cache_key);
		auto cached_integration_field = cached_fields.first;
		auto cached_flow_field = cached_fields.second;

		if (with_los) {
			log::log(SPAM << "Performing LOS pass on cached field");

			// Make a copy of the cached integration field
			auto integration_field = std::make_shared<IntegrationField<SECTOR_SIDE_LENGTH>>();

			// Only integrate LOS; leave the rest of the field as is
			integration_field->integrate_los(cost_field, other, other_sector_id, portal, target);

			log::log(SPAM << "Transferring LOS flags to cached flow field");

			// Make a copy of the cached flow field
			auto flow_field = std::make_shared<FlowField<SECTOR_SIDE_LENGTH>>(*cached_flow_field);

			// Transfer the LOS flags to the flow field
			flow_field->transfer_dynamic_flags(integration_field);

			return std::make_pair(integration_field, flow_field);
		}

		return std::make_pair(cached_integration_field, cached_flow_field);
	}

	auto integration_field = this->integrate(cost_field, other, other_sector_id, portal, target, time, with_los);
	auto flow_field = this->build(integration_field, other, other_sector_id, portal);

	log::log(DBG << "Caching integration and flow fields for portal ID: " << portal->get_id()
	             << ", sector ID: " << other_sector_id);

	// Copy the fields to the cache.
	std::shared_ptr<IntegrationField<SECTOR_SIDE_LENGTH>> cached_integration_field = std::make_shared<IntegrationField<SECTOR_SIDE_LENGTH>>();
	cached_integration_field->reset_dynamic_flags();

	std::shared_ptr<FlowField<SECTOR_SIDE_LENGTH>> cached_flow_field = std::make_shared<FlowField<SECTOR_SIDE_LENGTH>>(*flow_field);
	cached_flow_field->reset_dynamic_flags();

	field_cache_t<SECTOR_SIDE_LENGTH> field_cache = std::make_pair(cached_integration_field, cached_flow_field);

	this->field_cache->add(cache_key, field_cache);

	return std::make_pair(integration_field, flow_field);
}

} // namespace path
} // namespace openage
