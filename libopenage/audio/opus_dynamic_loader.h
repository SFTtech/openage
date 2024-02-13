// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <string>

#include "../util/path.h"
#include "dynamic_loader.h"
#include "opus_loading.h"
#include "types.h"


namespace openage {
namespace audio {


/**
 * A OpusDynamicLoader load's opus encoded data.
 */
class OpusDynamicLoader : public DynamicLoader {
private:
	/** The source file. */
	opus_file_t source;
	/** The resource's length in int16_t values. */
	size_t length;
	/** The resource's pcm channels. */
	int channels;

public:
	/**
	 * Creates a new OpusDynamicLoader.
	 * @param path the resource's location in the filesystem
	 */
	OpusDynamicLoader(const util::Path &path);
	virtual ~OpusDynamicLoader() = default;

	size_t load_chunk(int16_t *chunk_buffer,
	                  size_t offset,
	                  size_t chunk_size) override;
};

} // namespace audio
} // namespace openage
