// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <functional>

namespace qtsdl {

/**
 * Stores static properties during initialization to be able to assign them after all 'liveReloadTag' properties are set.
 */
class DeferredInitialConstantPropertyValues {
public:
	DeferredInitialConstantPropertyValues();

	/**
	 * Is virtual to be able to catch the error when non-persistent item uses liveReloadTag.
	 */
	virtual ~DeferredInitialConstantPropertyValues();

	/**
	 * Execute saved static properties assignments (for newly created cores).
	 */
	void apply_static_properties();

	/**
	 * Clear saved static properties assignments (for cores that existed before the GUI reload).
	 */
	void clear_static_properties();

	bool is_init_over() const;

protected:
	bool init_over;
	std::vector<std::function<void()>> static_properties_assignments;
};

} // namespace qtsdl
