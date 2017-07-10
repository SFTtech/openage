// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "internal/value_container.h"

namespace openage {
namespace curve {


/**
 * A Curve Object is a ordered collection of internal curves. Every sub-curve
 * needs to be registered at this parent element.
 */
class Object : public TriggerIntermediateMaster {
public:
	Object(TriggerFactory *factory) :
		TriggerIntermediateMaster(factory) {};

private:

};

}} // openage :: curve
