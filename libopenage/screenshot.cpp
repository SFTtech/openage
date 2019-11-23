// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#include "screenshot.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <epoxy/gl.h>
#include <memory>
#include <png.h>

#include "coord/pixel.h"
#include "job/job_manager.h"
#include "log/log.h"
#include "util/strings.h"

namespace openage {


ScreenshotManager::ScreenshotManager(job::JobManager *job_mgr)
	:
	count{0},
	job_manager{job_mgr} {
}


ScreenshotManager::~ScreenshotManager() {}


std::string ScreenshotManager::gen_next_filename() {

	std::time_t t = std::time(NULL);

	if (t == this->last_time) {
		this->count++;
	} else {
		this->count = 0;
		this->last_time = t;
	}

	// these two values (32) *must* be the same for safety reasons
	char timestamp[32];
	std::strftime(timestamp, 32, "%Y-%m-%d_%H-%M-%S", std::localtime(&t));

	return util::sformat("/tmp/openage_%s_%02d.png", timestamp, this->count);
}


void ScreenshotManager::save_screenshot(coord::viewport_delta size) {
	coord::pixel_t width = size.x,
	               height = size.y;

	std::shared_ptr<uint8_t> pxdata(new uint8_t[4*width*height], std::default_delete<uint8_t[]>());
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pxdata.get());

	auto encode_function = [this, pxdata, size] () {
		return this->encode_png(pxdata, size);
	};
	this->job_manager->enqueue<bool>(encode_function);
}


bool ScreenshotManager::encode_png(std::shared_ptr<uint8_t> pxdata,
                                   coord::viewport_delta size) {
	std::FILE *fout = NULL;
	coord::pixel_t width = size.x,
	               height = size.y;
	auto warn_fn = [] (png_structp /*png_ptr*/, png_const_charp message) {
		log::log(MSG(err) << "Creating screenshot failed: libpng error: " << message);
	};
	auto err_fn = [] (png_structp png_ptr, png_const_charp message) {
		log::log(MSG(err) << "Creating screenshot failed: libpng error: " << message);
		longjmp(png_jmpbuf(png_ptr), 1);
	};

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
	                                              (png_voidp) NULL,
	                                              err_fn, warn_fn);
	if (!png_ptr)
		return false;

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		std::fclose(fout);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return false;
	}

	std::string filename = this->gen_next_filename();
	fout = std::fopen(filename.c_str(), "wb");
	if (fout == NULL) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		log::log(MSG(err) << "Could not open '"<< filename << "': "
		         << std::string(strerror(errno)));
		return false;
	}

	png_init_io(png_ptr, fout);

	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
	             PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
	             PNG_FILTER_TYPE_DEFAULT);

	// Put image row pointer into info_ptr so that we can use the high-level
	// write interface.
	std::vector<png_bytep> row_ptrs;

	// Invert rows.
	row_ptrs.reserve(height);
	for (int i = 1; i <= height; i++) {
		row_ptrs.push_back(pxdata.get() + (height - i) * 4 * width);
	}
	png_set_rows(png_ptr, info_ptr, &row_ptrs[0]);

	//TODO: print ingame message.
	log::log(MSG(info) << "Saving screenshot to '" << filename << "'.");

	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_FILLER_AFTER, NULL);

	std::fclose(fout);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	return true;
}

} // openage
