// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

namespace openage {
namespace curve {

#include "internal/value_container.h"

/**
 * A Curve Object is a ordered collection of internal curves. Every sub-curve
 * needs to be registered at this parent element.
 */
class Object : public Trigger {
public:
	Object(TriggerFactory *factory) :
		Trigger(factory, parent) {};

private:

};

}} // openage :: curve
