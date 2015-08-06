// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "resource.h"

#include "../engine.h"
#include "../error/error.h"

#include "dynamic_resource.h"
#include "in_memory_resource.h"

namespace openage {
namespace audio {


Resource::Resource(category_t category, int id)
		:
		category{category},
		id{id} {
}


category_t Resource::get_category() const {
	return category;
}


int Resource::get_id() const {
	return id;
}


void Resource::use() {}


void Resource::stop_using() {}


std::shared_ptr<Resource> Resource::create_resource(category_t category, int id,
                                                    const std::string &path,
                                                    format_t format,
                                                    loader_policy_t loader_policy) {
	switch (loader_policy) {
	case loader_policy_t::IN_MEMORY:
		return std::make_shared<InMemoryResource>(category, id, path, format);
	case loader_policy_t::DYNAMIC:
		return std::make_shared<DynamicResource>(category, id, path, format);
	default:
		throw Error{MSG(err) << "Unsupported loader policy: " << loader_policy};
	}
}


}} // namespace openage::audio
