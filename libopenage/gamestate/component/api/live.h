// Copyright 2021-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdint>
#include <memory>

#include <nyan/nyan.h>

#include "curve/container/map.h"
#include "gamestate/component/api_component.h"
#include "gamestate/component/types.h"
#include "time/time.h"


namespace openage {

namespace curve {
template <KeyframeValueLike T>
class Segmented;
} // namespace curve

namespace gamestate::component {
class Live final : public APIComponent {
public:
	using APIComponent::APIComponent;

	component_t get_type() const override;

	/**
	 * Add a new attribute to the component attributes.
	 *
	 * @param time The time at which the attribute is added.
	 * @param attribute Attribute identifier (fqon of the nyan object).
	 * @param starting_values Attribute values at the time of addition.
	 */
	void add_attribute(const time::time_t &time,
	                   const nyan::fqon_t &attribute,
	                   std::shared_ptr<curve::Segmented<attribute_value_t>> starting_values);

	/**
	 * Get the value of an attribute at a given time.
	 *
	 * @param time The time at which the attribute is queried.
	 * @param attribute Attribute identifier (fqon of the nyan object).
	 *
	 * @return Value of the attribute at the given time.
	 */
	const attribute_value_t get_attribute(const time::time_t &time,
	                                      const nyan::fqon_t &attribute) const;

	/**
	 * Set the value of an attribute at a given time.
	 *
	 * @param time The time at which the attribute is set.
	 * @param attribute Attribute identifier (fqon of the nyan object).
	 * @param value New attribute value.
	 */
	void set_attribute(const time::time_t &time,
	                   const nyan::fqon_t &attribute,
	                   attribute_value_t value);

private:
	using attribute_storage_t = curve::UnorderedMap<nyan::fqon_t,
	                                                std::shared_ptr<curve::Segmented<attribute_value_t>>>;

	/**
	 * Map of attribute values by attribute type.
	 */
	attribute_storage_t attributes;
};

} // namespace gamestate::component
} // namespace openage
