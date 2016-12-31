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

/** function type used to get a configuration value */
using get_func = std::function<std::string()>;

/** function type to set the value of a config entry */
using set_func = std::function<void(std::string)>;


/**
 * Configuration manager.
 *
 * Stores key-value pairs of config data.
 * Actually it doesn't store data, instead functions
 * that perform/fetch the configuration at the appropriate place.
 *
 * pxd:
 *
 * cppclass CVarManager:
 *
 *     string get(string name) except +
 *     void set(string name, string value) except +
 *     void load_config(string path) except +
 */
class CVarManager {

public:
	/**
	 * Creates a configuration entry
	 * @returns if the entry name was successful.
	 *          It won't be if the name was used before.
	 */
	bool create(const std::string &name,
	            const std::pair<get_func, set_func> &accessors);

	/**
	 * Gets the value of a config entry.
	 * Internally calls the stored get function.
	 */
	std::string get(const std::string &name) const;

	/**
	 * Sets the config entry value.
	 */
	void set(const std::string &name, const std::string &value) const;

	/**
	 * Returns the path to the main config file.
	 */
	std::string find_main_config() const;

	/**
	 * Performs the loading of a configuration file
	 * via the Python implementation.
	 */
	void load_config(const std::string &path);

	/**
	 * Perform the load of the main configuration file.
	 */
	void load_main_config();

private:
	/**
	 * Store the key-value pair of config options.
	 * The clue is to store the "what does the config do",
	 * not the actual value.
	 * That way the system is universal.
	 */
	std::unordered_map<std::string, std::pair<get_func, set_func>> store;
};


/**
 * Python function to locate the main openage configuration file.
 * Provides the file name.
 *
 * pxd: PyIfFunc0[string] find_main_config_file
 */
extern pyinterface::PyIfFunc<std::string> find_main_config_file;


/**
 * Python function to load a configuration file.
 * The config manager is passed into it.
 *
 * pxd: PyIfFunc2[void, string, CVarManager*] load_config_file
 */
extern pyinterface::PyIfFunc<void, std::string, CVarManager*> load_config_file;

}} // openage::cvar
