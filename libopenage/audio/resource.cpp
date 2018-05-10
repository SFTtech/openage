// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "resource.h"

#include "../engine.h"
#include "../error/error.h"

#include "dynamic_resource.h"
#include "in_memory_resource.h"

namespace openage {
namespace audio {


Resource::Resource(AudioManager *manager, category_t category, int id)
	:
	manager{manager},
	category{category},
	id{id} {}


category_t Resource::get_category() const {
	return category;
}


int Resource::get_id() const {
	return id;
}


std::shared_ptr<Resource> Resource::create_resource(AudioManager *manager,
                                                    const resource_def &def) {

	if (unlikely(not def.location.is_file())) {
		throw Error{ERR << "sound file does not exist: " << def.location};
	}

	switch (def.loader_policy) {
	case loader_policy_t::IN_MEMORY:
		return std::make_shared<InMemoryResource>(
			manager, def.category, def.id, def.location, def.format
		);

	case loader_policy_t::DYNAMIC:
		return std::make_shared<DynamicResource>(
			manager, def.category, def.id, def.location, def.format
		);

	default:
		throw Error{ERR << "Unsupported loader policy: " << def.loader_policy};
	}
}


}} // namespace openage::audio
