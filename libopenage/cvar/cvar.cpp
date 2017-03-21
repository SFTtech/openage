// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "cvar.h"

#include "../log/log.h"


namespace openage {
namespace cvar {

CVarManager::CVarManager(const util::Path &path)
	:
	path{path} {}


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
	auto it = this->store.find(name);
	if (it != this->store.end()) {
		return it->second.first();
	}
	return "";
}


void CVarManager::set(const std::string &name, const std::string &value) const {
	auto it = store.find(name);
	if (it != store.end()) {
		it->second.second(value);
	}
}


void CVarManager::load_config(const util::Path &path) {
	pyx_load_config_file.call(this, path);
}


void CVarManager::load_all() {
	log::log(INFO << "loading configuration files...");
	this->load_config(this->path["keybinds.oac"]);
}

pyinterface::PyIfFunc<void, CVarManager *, const util::Path &> pyx_load_config_file;

}} // openage::cvar
