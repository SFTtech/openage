#ifndef _ENGINE_AUDIO_DYNAMIC_LOADER_H_
#define _ENGINE_AUDIO_DYNAMIC_LOADER_H_

#include <memory>
#include <string>

#include <opusfile.h>

#include "format.h"
#include "types.h"

namespace engine {
namespace audio {

class DynamicLoader {
protected:
	std::string path;

public:
	DynamicLoader(const std::string &path);
	virtual ~DynamicLoader() = default;

	virtual uint32_t get_length() = 0;
	virtual pcm_data_t load_chunk(uint32_t position, uint32_t length) = 0;

	static std::unique_ptr<DynamicLoader> create(const std::string &path,
			format_t format);
};

using opus_file_t =
		std::unique_ptr<OggOpusFile,std::function<void(OggOpusFile*)>>;

class OpusDynamicLoader : public DynamicLoader {
private:
	uint32_t length;
	int channels;

public:
	OpusDynamicLoader(const std::string &path);
	virtual ~OpusDynamicLoader() = default;

	virtual uint32_t get_length();
	virtual pcm_data_t load_chunk(uint32_t position, uint32_t length);

private:
	opus_file_t open_opus_file();
};

}
}

#endif
