// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "opus_loading.h"

#include "error.h"
#include "../log/log.h"


namespace openage {
namespace audio {

// custom deleter for OggOpusFile unique pointers
static auto opus_deleter = [](OggOpusFile *op_file) {
	if (op_file != nullptr) {
		op_free(op_file);
	}
};


static op_read_func opus_reader = [](void *stream,
                                     unsigned char *buf,
                                     int count) -> int {

	util::File *file = reinterpret_cast<util::File *>(stream);
	return file->read_to(buf, count);
};


static op_seek_func opus_seeker = [](void *stream,
                                     opus_int64 offset,
                                     int whence) -> int {

	util::File *file = reinterpret_cast<util::File *>(stream);
	if (unlikely(not file->seekable())) {
		return -1;
	}

	file->seek(offset, static_cast<util::File::seek_t>(whence));
	return 0;
};


static op_tell_func opus_teller = [](void *stream) -> opus_int64 {
	util::File *file = reinterpret_cast<util::File *>(stream);
	return file->tell();
};


static OpusFileCallbacks opus_access_funcs{
	opus_reader,
	opus_seeker,
	opus_teller,
	nullptr        // no close function needed (as opus_file_t owns the File)
};


opus_file_t open_opus_file(const util::Path &path) {
	if (not path.is_file()) {
		throw audio::Error{
			ERR << "Audio file not found: " << path
		};
	}

	opus_file_t op_file;
	int op_err = 0;

	//asm("int $3");

	// check if the file can be opened directly
	auto native_path = path.resolve_native_path();
	if (false && native_path.size() > 0) {

		op_file.handle = {
			op_open_file(native_path.c_str(), &op_err),
			opus_deleter
		};
	}
	else {
		// open the file and move the handle to the heap
		op_file.file = std::make_unique<util::File>();
		*op_file.file = path.open_r();

		op_file.handle = {
			op_open_callbacks(op_file.file.get(), &opus_access_funcs,
			                  nullptr, 0, &op_err),
			opus_deleter
		};
	}

	if (op_err != 0) {

		const char *reason;

		switch (op_err) {
		case OP_EREAD:
			reason = "read/seek/tell failed or data has changed"; break;
		case OP_EFAULT:
			reason = "opus failed to allocate memory "
			         "or something else bad happened internally"; break;
		case OP_EIMPL:
			reason = "Stream used an unsupported feature"; break;
		case OP_EINVAL:
			reason = "seek() worked, but tell() did not, "
			         "or initial_bytes != start seek pos"; break;
		case OP_ENOTFORMAT:
			reason = "Data didn't contain opus stream"; break;
		case OP_EBADHEADER:
			reason = "Header packet was invalid or missing"; break;
		case OP_EVERSION:
			reason = "ID header has unrecognized version"; break;
		case OP_EBADLINK:
			reason = "Data we already saw before seeking not found"; break;
		case OP_EBADTIMESTAMP:
			reason = "Validity check for first/last timestamp failed"; break;

		default:
			reason = "Unknown other error in opusfile"; break;
		}

		throw audio::Error{
			ERR
			<< "Could not open opus file: "
			<< path << " = '" << native_path << "': "
			<< reason
		};
	}
	return op_file;
}

}} // openage::audio
