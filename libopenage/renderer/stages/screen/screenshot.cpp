// Copyright 2014-2024 the openage authors. See copying.md for legal info.

#include "screenshot.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <epoxy/gl.h>
#include <memory>
#include <png.h>

#include "job/job_manager.h"
#include "log/log.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/resources/texture_data.h"
#include "renderer/stages/screen/render_stage.h"
#include "util/strings.h"


namespace openage::renderer::screen {


ScreenshotManager::ScreenshotManager(std::shared_ptr<ScreenRenderStage> &renderer,
                                     util::Path &outdir,
                                     std::shared_ptr<job::JobManager> &job_mgr) :
	outdir{outdir},
	count{0},
	last_time{0},
	renderer{renderer},
	job_manager{job_mgr} {
}

std::string ScreenshotManager::gen_next_filename() {
	std::time_t t = std::time(NULL);

	if (t == this->last_time) {
		this->count++;
	}
	else {
		this->count = 0;
		this->last_time = t;
	}

	// these two values (32) *must* be the same for safety reasons
	char timestamp[32];
	std::strftime(timestamp, 32, "%Y-%m-%d_%H-%M-%S", std::localtime(&t));

	return util::sformat("openage_%s_%02d.png", timestamp, this->count);
}


void ScreenshotManager::save_screenshot() {
	// get screenshot image from scren renderer
	auto pass = this->renderer->get_render_pass();
	auto target = pass->get_target();
	auto image = target->into_data();

	auto store_function = [this, image]() {
		image.store(this->outdir / this->gen_next_filename());
		return true;
	};
	this->job_manager->enqueue<bool>(store_function);
}

} // namespace openage::renderer::screen
