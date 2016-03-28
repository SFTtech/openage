// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <stdlib.h>
// pxd: from libcpp.string cimport string
#include <string>
#include <unordered_map>
#include <utility>

// pxd: from libopenage.pyinterface.functional cimport PyIfFunc0, PyIfFunc2
#include "../pyinterface/functional.h"

namespace openage {
namespace cvar {

using get_func = std::function<std::string()>;
using set_func = std::function<void(std::string)>;

/**
 * pxd:
 *
 * cppclass CVarManager:
 *
 *     string get(string name) except +
 *     void set(string name, string value) except +
 *     void load_config_file(string path) except +
 *     @staticmethod
 *     CVarManager &get() except +
 *     PyIfFunc2[void, string, CVarManager*] load_config_file
 *     PyIfFunc0[string] find_main_config_file
 */
class CVarManager {

public :
	bool create(const std::string &name, const std::pair<get_func, set_func> &accessors);
	std::string get(const std::string &name) const;
	void set(const std::string &name, const std::string &value) const;
	std::string find_main_config() const;
	void load_config(const std::string &path);
	void load_main_config();
	static CVarManager &get();
	static pyinterface::PyIfFunc<std::string> find_main_config_file;
	static pyinterface::PyIfFunc<void, std::string, CVarManager*> load_config_file;

private :
	std::unordered_map<std::string, std::pair<get_func, set_func>> store;
};

}
}
