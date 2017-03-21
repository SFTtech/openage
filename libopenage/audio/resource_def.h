// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>

#include "../util/path.h"
#include "category.h"
#include "format.h"
#include "loader_policy.h"


namespace openage {
namespace audio {


/**
 * Definition of a single sound resource file to load.
 */
struct resource_def {
	category_t category;
	int id;
	util::Path location;
	format_t format;
	loader_policy_t loader_policy;
};


}} // openage::audio
