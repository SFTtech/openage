#ifndef _ENGINE_AUDIO_DYNAMIC_LOADER_H_
#define _ENGINE_AUDIO_DYNAMIC_LOADER_H_

#include <memory>
#include <string>

#include <opusfile.h>

#include "format.h"
#include "types.h"

namespace engine {
namespace audio {

/**
 * A DynamicLoader loads pcm chunks without loading the whole resource. A chunk
 * is a int16_t buffer with a fixed size that contains 16 bit signed integer
 * pcm data.
 */
class DynamicLoader {
protected:
	/**
	 * the resource's location in the filesystem
	 */
	std::string path;

public:
	/**
	 * Initializes a new DynamicLoader.
	 * @param path the resource's location in the filesystem
	 */
	DynamicLoader(const std::string &path);
	virtual ~DynamicLoader() = default;

	/**
	 * Returns the resource's length in int16_t values.
	 */
	virtual uint32_t get_length() = 0;
	/**
	 * Loads a chunk of stereo pcm data from the resource. The chunk of data
	 * begins at the given offset from the beginning of the resource and the
	 * maximum length is supplied.
	 * @param offset the offset from the resource's beginning
	 * @param chunk_size the number of int16_t values that fit in one chunk
	 */
	virtual pcm_chunk_t load_chunk(uint32_t offset, uint32_t chunk_size) = 0;

	/**
	 * Creates a DynamicLoader instance that supports the given format.
	 * @param path the resource's location in the filesystem
	 * @param format the resource's audio format
	 */
	static std::unique_ptr<DynamicLoader> create(const std::string &path,
			format_t format);
};


/**
 * A OpusDynamicLoader load's opus encoded data.
 */
class OpusDynamicLoader : public DynamicLoader {
private:
	/**
	 * The resource's length in int16_t values.
	 */
	uint32_t length;
	/**
	 * The resource's pcm channels.
	 */
	int channels;

public:
	/**
	 * Creates a new OpusDynamicLoader.
	 * @param path the resource's location in the filesystem
	 */
	OpusDynamicLoader(const std::string &path);
	virtual ~OpusDynamicLoader() = default;

	virtual uint32_t get_length();
	virtual pcm_chunk_t load_chunk(uint32_t offset, uint32_t chunk_size);

private:
	/**
	 * Opens a opus file. Its location is specified by the path stored in the
	 * DynamicLoader.
	 */
	opus_file_t open_opus_file();
};

}
}

#endif
