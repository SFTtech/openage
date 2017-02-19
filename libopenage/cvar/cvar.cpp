// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "cvar.h"

#include <tuple>

#include "../log/log.h"


namespace openage {
namespace cvar {

CVarManager::CVarManager(const util::Path &path)
	:
	path{path},
	config_loading{} {
}


bool CVarManager::create(const std::string &name,
                         const std::pair<get_func, set_func> &accessors) {
	auto it = this->store.find(name);
	if (it == this->store.end()) {
		this->store[name] = accessors;
		return true;
	}
	return false;
}


std::string CVarManager::get(const std::string &name) const {
	std::lock_guard<std::mutex> lock(this->store_mutex);

	auto it = this->store.find(name);
	if (it != this->store.end()) {
		return it->second.first();
	}
	return "";
}


std::vector<std::string> CVarManager::get_names() const {
	std::vector<std::string> result;
	result.reserve(this->store.size());

	std::lock_guard<std::mutex> lock(this->store_mutex);

	for (const auto &opt : this->store) {
		result.emplace_back(std::get<const std::string>(opt));
	}

	return result;
}


void CVarManager::set(const std::string &name, const std::string &value) const {
	set_func setter;

	{
		std::lock_guard<std::mutex> lock(this->store_mutex);

		auto it = store.find(name);
		if (it != store.end()) {
			setter = it->second.second;
		}
	}

	if (setter) {
		setter(value);

		if (!this->config_loading) {
			pyx_config_file_set_option.call(this->path["keybinds.oac"], name, value);
		}
	}
}


void CVarManager::load_config(const util::Path &path) {
	this->config_loading = true;
	pyx_load_config_file.call(this, path);
	this->config_loading = false;
}


void CVarManager::load_all() {
	log::log(INFO << "loading configuration files...");
	this->load_config(this->path["keybinds.oac"]);
}

pyinterface::PyIfFunc<void, CVarManager *, const util::Path &> pyx_load_config_file;
pyinterface::PyIfFunc<void, const util::Path &, std::string, std::string> pyx_config_file_set_option;

}} // openage::cvar
