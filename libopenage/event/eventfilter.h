// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <functional>

#include "eventtarget.h"


namespace openage::event {

/**
 * Store a filter function that can then be applied on an event target.
 */
class EventFilter {
public:
	EventFilter(std::function<bool(const std::shared_ptr<EventTarget> &)> filter);

	bool apply(const std::shared_ptr<EventTarget> &target) const;

private:
	std::function<bool(const std::shared_ptr<EventTarget>&)> filter;
};


} // namespace openage::event
