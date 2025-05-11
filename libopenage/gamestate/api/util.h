// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

#include <nyan/nyan.h>


namespace openage::gamestate::api {

/**
 * Get the file path from a reference in \p file member of a nyan object.
 *
 * File references in \p file members can be relative (to the nyan object)
 * or absolute (relative to a modpack mount folder). See
 * https://github.com/SFTtech/openage/blob/master/doc/media/openage/file_referencing.md
 * for more info.
 *
 * @param obj nyan object where the file reference is stored.
 * @param path File reference (relative or absolute).
 *
 * @return Relative file path from the modpack root folder.
 */
const std::string resolve_file_path(const nyan::Object &obj, const std::string &path);

/**
 * Get the fqon of the first parent of the object that is defined in the
 * API namespace (i.e. it's part of the \p engine namespace).
 *
 * If the object itself is part of the API namespace, it will return the fqon
 * of the object.
 *
 * @param obj nyan object.
 *
 * @return fqon of the first parent in the API namespace.
 * @throws Error if the object has no parents in the API namespace.
 */
const nyan::fqon_t &get_api_parent(const nyan::Object &obj);

} // namespace openage::gamestate::api
