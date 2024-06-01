// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <unordered_map>

#include "pathfinding/types.h"
#include "util/hash.h"


namespace openage {
namespace coord {
struct tile_delta;
} // namespace coord

namespace path {
class CostField;
class FlowField;
class IntegrationField;
class Portal;

/**
 * Integrator for the flow field pathfinding algorithm.
 */
class Integrator {
public:
	Integrator() = default;
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
	std::shared_ptr<IntegrationField> integrate(const std::shared_ptr<CostField> &cost_field,
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
	 * @param with_los If true an LOS pass is performed before cost integration.
	 *
	 * @return Integration field.
	 */
	std::shared_ptr<IntegrationField> integrate(const std::shared_ptr<CostField> &cost_field,
	                                            const std::shared_ptr<IntegrationField> &other,
	                                            sector_id_t other_sector_id,
	                                            const std::shared_ptr<Portal> &portal,
	                                            const coord::tile_delta &target,
	                                            bool with_los = true);

	/**
	 * Build the flow field from an integration field.
	 *
	 * @param integration_field Integration field.
	 *
	 * @return Flow field.
	 */
	std::shared_ptr<FlowField> build(const std::shared_ptr<IntegrationField> &integration_field);

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
	std::shared_ptr<FlowField> build(const std::shared_ptr<IntegrationField> &integration_field,
	                                 const std::shared_ptr<IntegrationField> &other,
	                                 sector_id_t other_sector_id,
	                                 const std::shared_ptr<Portal> &portal,
	                                 bool with_los = true);

	using get_return_t = std::pair<std::shared_ptr<IntegrationField>, std::shared_ptr<FlowField>>;

	/**
	 * Get the integration field and flow field for a target.
	 *
	 * @param cost_field Cost field.
	 * @param target Coordinates of the target cell.
	 *
	 * @return Integration field and flow field.
	 */
	get_return_t get(const std::shared_ptr<CostField> &cost_field,
	                 const coord::tile_delta &target);

	/**
	 * Get the integration field and flow field from a portal.
	 *
	 * @param cost_field Cost field.
	 * @param other Integration field of the other side of the portal.
	 * @param other_sector_id Sector ID of the other side of the portal.
	 * @param portal Portal.
	 * @param target Coordinates of the target cell, relative to the integration field origin.
	 * @param with_los If true an LOS pass is performed before cost integration.
	 *
	 * @return Integration field and flow field.
	 */
	get_return_t get(const std::shared_ptr<CostField> &cost_field,
	                 const std::shared_ptr<IntegrationField> &other,
	                 sector_id_t other_sector_id,
	                 const std::shared_ptr<Portal> &portal,
	                 const coord::tile_delta &target,
	                 bool with_los = true);

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
		field_cache;
};

} // namespace path
} // namespace openage
