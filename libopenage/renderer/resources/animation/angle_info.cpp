// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "angle_info.h"

namespace openage::renderer::resources {

AngleInfo::AngleInfo(const float angle_start,
                     std::vector<std::shared_ptr<FrameInfo>> &frames,
                     std::shared_ptr<AngleInfo> mirror_from,
                     flip_type mirror_type) :
	angle_start{angle_start},
	frames{frames},
	mirror_from{mirror_from},
	mirror_type{mirror_type} {}

float AngleInfo::get_angle_start() const {
	return this->angle_start;
}

bool AngleInfo::is_mirrored() const {
	return this->mirror_from != nullptr;
}

const flip_type &AngleInfo::get_mirror_type() {
	return this->mirror_type;
}

const std::shared_ptr<AngleInfo> &AngleInfo::get_mirrored_angle() const {
	return this->mirror_from;
}

size_t AngleInfo::get_frame_count() const {
	return this->frames.size();
}

const std::shared_ptr<FrameInfo> &AngleInfo::get_frame(size_t idx) const {
	if (!(this->mirror_from == nullptr)) {
		return this->mirror_from->get_frame(idx);
	}
	return this->frames.at(idx);
}

} // namespace openage::renderer::resources
