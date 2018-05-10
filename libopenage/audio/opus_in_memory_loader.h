// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include "in_memory_loader.h"
#include "types.h"
#include "../util/path.h"


namespace openage {
namespace audio {

/**
 * A OpusInMemoryLoader load's opus encoded data from opus files. A opus file is
 * loaded completely at once and the loaded data will be kept in memory.
 */
class OpusInMemoryLoader : public InMemoryLoader {
public:
	/**
	 * Creates a new OpusInMemoryLoader.
	 * @param path the resource's location in the filesystem
	 */
	OpusInMemoryLoader(const util::Path &path);
	virtual ~OpusInMemoryLoader() = default;

	pcm_data_t get_resource() override;
};

}} // openage::audio
